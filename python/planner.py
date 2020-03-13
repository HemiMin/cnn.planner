r"""
e-PlaNNer frontend
"""
import os
import subprocess
import exceptions
import torch.nn as nn
import torch

class Planner:
    r"""
    e-PlaNNer frontend class
    """

    # pylint: disable=too-many-instance-attributes
    # DNN parameter requires too many attributes.

    def __init__(self, verbose=False, debug=False, output_dir='output'):

        print_logo()

        self.batch = None
        self.stride = None

        self.input_width = None
        self.input_height = None
        self.input_channel = None

        self.padding_width = None
        self.padding_height = None

        self.kernel_width = None
        self.kernel_height = None

        self.output_width = None
        self.output_height = None
        self.output_channel = None

        self.mac_cycles = None
        self.frequency = None
        self.bandwidth = None
        self.mac_energy = None
        self.on_chip_energy = None
        self.off_chip_energy = None
        self.input_mem_size = None
        self.weight_mem_size = None
        self.output_mem_size = None
        self.pe_dim = None
        self.pe_strt = None

        self.output_dir = output_dir
        if not os.path.isdir(self.output_dir):
            print('[Front-end] Make output directory: {}/'.format(self.output_dir))
            os.mkdir(self.output_dir)
        self.gen_code = None
        self.gaia_code = None
        self.sim_binary = None
        self.latency_file = None
        self.timestamp_file = None
        self.tiling_dump = None
        self.loop_seq_dump = None
        self.report_file = None
        self.layer_name = None

        self.data = None
        self.module = None

        self.verbosity = verbose
        self.debug = debug

        self.log_dir = 'log'

    def set_data(self, **kwargs):
        r'''
        Set dataset on the Planner object.

        **kwargs
        - data: 4-dimension input data generated by torch (Required)
        - module: layer object generated by torch.nn.Module (Required)
        - hw_spec: hardware configurations (Required)
        - layer_name: CNN layer name (Required)
        '''

        # pylint: disable=too-many-statements
        # I need many statements.

        self.layer_name = kwargs['layer_name']

        self.module = kwargs['module']
        self.data = kwargs['data']

        print('\n[Front-end] \x1b[32m' + self.layer_name + ' start...\x1b[0m')

        self.gen_code = self.output_dir + '/' + self.layer_name + '.cc'
        self.gaia_code = self.output_dir + '/' + self.layer_name + '.gaia'
        self.sim_binary = self.output_dir + '/' + self.layer_name + '_sim'
        self.latency_file = self.output_dir + '/' + self.layer_name + '.vl'
        self.timestamp_file = self.output_dir + '/' + self.layer_name + '.json'
        self.tiling_dump = self.output_dir + '/' + self.layer_name + '_tiling.dump'
        self.loop_seq_dump = self.output_dir + '/' + self.layer_name + '_loop_seq.dump'
        self.report_file = self.output_dir + '/' + 'report.csv'

        if self.data.shape[2] != self.data.shape[3]:
            raise exceptions.ValueException

        self.batch = self.data.shape[0]
        self.input_channel = self.data.shape[1]
        self.input_width = self.data.shape[2]
        self.input_height = self.data.shape[3]

        if isinstance(self.module, nn.Conv2d):
            if self.data.shape[1] != self.module.in_channels:
                raise exceptions.ValueException

            self.stride = self.module.stride[0]

            self.input_width = self.input_width
            self.input_height = self.input_height

            self.padding_width = self.module.padding[1]
            self.padding_height = self.module.padding[0]

            self.kernel_width = self.module.kernel_size[1]
            self.kernel_height = self.module.kernel_size[0]

            self.output_channel = self.module.out_channels
        elif isinstance(self.module, nn.Linear):
            self.stride = 1

            if self.input_width != 1:
                self.input_channel = self.input_width * self.input_height * self.input_channel

            if self.input_channel != self.module.in_features:
                raise exceptions.ValueException

            self.input_width = 1
            self.input_height = 1

            self.kernel_width = 1
            self.kernel_height = 1

            self.output_channel = self.module.out_features
        elif isinstance(self.module, (nn.MaxPool2d, nn.AdaptiveAvgPool2d)):
            if isinstance(self.module, nn.AdaptiveAvgPool2d):
                self.stride = 1
            else:
                self.stride = self.module.stride

            if isinstance(self.module, nn.AdaptiveAvgPool2d):
                self.padding_width = 0
                self.padding_height = 0
            else:
                self.padding_width = self.module.padding
                self.padding_height = self.module.padding

            if isinstance(self.module, nn.AdaptiveAvgPool2d):
                self.kernel_width = self.input_width
                self.kernel_height = self.input_height
            else:
                self.kernel_width = self.module.kernel_size
                self.kernel_height = self.module.kernel_size

            self.output_channel = self.input_channel
        elif isinstance(self.module, nn.ReLU):
            self.stride = 1

            self.kernel_width = 1
            self.kernel_height = 1

            self.output_channel = self.input_channel
        elif isinstance(self.module, nn.Dropout):
            self.stride = 1

            self.kernel_width = 1
            self.kernel_height = 1

            self.output_channel = self.input_channel

        self.mac_cycles = kwargs['hw_spec'].mac_cycles
        self.frequency = kwargs['hw_spec'].frequency
        self.bandwidth = kwargs['hw_spec'].bandwidth
        self.mac_energy = kwargs['hw_spec'].mac_energy
        self.on_chip_energy = kwargs['hw_spec'].on_chip_energy_32
        self.off_chip_energy = kwargs['hw_spec'].off_chip_energy_32
        self.input_mem_size = kwargs['hw_spec'].mem_size[0]
        self.weight_mem_size = kwargs['hw_spec'].mem_size[1]
        self.output_mem_size = kwargs['hw_spec'].mem_size[2]
        self.pe_dim = kwargs['hw_spec'].pe_dim
        self.pe_strt = kwargs['hw_spec'].pe_strt

    def __make_compiler_argv(self, presched):
        argv = self.__make_argv()

        if presched:
            argv.append('-p')

        argv.append('--code-path=' + str(self.gen_code))
        argv.append('--gaia-path=' + str(self.gaia_code))
        argv.append('--timestamp-path=' + str(self.timestamp_file))

        return argv

    def __make_profiler_argv(self):
        argv = self.__make_argv()

        argv.append('--mac-energy=' + str(self.mac_energy))
        argv.append('--on-chip-32-energy=' + str(self.on_chip_energy))
        argv.append('--off-chip-32-energy=' + str(self.off_chip_energy))

        argv.append('--report-path=' + str(self.report_file))

        if self.verbosity:
            argv.append('-v')
        if self.debug:
            argv.append('-g')

        return argv

    def __make_argv(self):
        argv = []

        argv.append('--stride=' + str(self.stride))

        argv.append('--iw=' + str(self.input_width))
        argv.append('--ih=' + str(self.input_height))
        argv.append('--ic=' + str(self.input_channel))
        argv.append('--pw=' + str(self.padding_width))
        argv.append('--ph=' + str(self.padding_height))
        argv.append('--kw=' + str(self.kernel_width))
        argv.append('--kh=' + str(self.kernel_height))
        argv.append('--oc=' + str(self.output_channel))

        argv.append('--mac-cycles=' + str(self.mac_cycles))
        argv.append('--frequency=' + str(self.frequency))
        argv.append('--bandwidth=' + str(self.bandwidth))
        argv.append('--input-mem-size=' + str(self.input_mem_size))
        argv.append('--weight-mem-size=' + str(self.weight_mem_size))
        argv.append('--output-mem-size=' + str(self.output_mem_size))
        argv.append('--pe-dim=' + str(self.pe_dim))
        argv.append('--pe-structure=' + str(self.pe_strt))

        argv.append('--latency-path=' + str(self.latency_file))
        argv.append('--tiling-dump=' + str(self.tiling_dump))
        argv.append('--loop-seq-dump=' + str(self.loop_seq_dump))

        argv.append('--layer=' + str(self.layer_name))

        return argv

    def __run_compiler(self, exe_bin_path, presched):
        print('[Front-end][Compiler] Compilation start...')
        argv = self.__make_compiler_argv(presched)
        return subprocess.Popen([exe_bin_path + str('/compiler')] + argv).wait()

    def __compile_code(self):
        print('[Front-end][Compiler] Generated source code compile...')
        cxx = 'g++'
        cxx_flags = '-std=c++11 -Ofast -Wall -Werror -o ' + self.sim_binary + ' ' + self.gen_code
        return subprocess.Popen([cxx] + cxx_flags.split()).wait()

    def __run_binary(self):
        print('[Front-end][Simulation] Run simulation code...')
        return subprocess.Popen(['./' + self.sim_binary]).wait()

    def __run_profiler(self, exe_bin_path):
        print('[Front-end][Profiler] Profiling start...')
        argv = self.__make_profiler_argv()
        return subprocess.Popen([exe_bin_path + str('/profiler')] + argv).wait()

    def run(self, exe_bin_path='.', presched=False, baseline=0):
        r'''
        Run CNN planner
        '''

        exit_failure = 1
        self.output_width = None
        self.output_height = None

        if not os.path.isdir(self.log_dir):
            print('[Front-end] Make log directory: {}/'.format(self.log_dir))
            os.mkdir(self.log_dir)

        if isinstance(self.module, (nn.Conv2d, nn.Linear)):
            self.output_width = (self.input_width + 2*self.padding_width - self.kernel_width) // self.stride + 1
            self.output_height = (self.input_height + 2*self.padding_height - self.kernel_height)//self.stride + 1
            if self.__run_compiler(exe_bin_path, presched) is exit_failure:
                raise exceptions.CodeGenerationFail
            elif self.__compile_code() is exit_failure:
                raise exceptions.CodeCompileFail
            elif self.__run_binary() is exit_failure:
                raise exceptions.RunCodeFail
            elif self.__run_profiler(exe_bin_path) is exit_failure:
                raise exceptions.ReportFail

        elif isinstance(self.module, nn.MaxPool2d):
            print('[Front-end] MaxPool2d is not supported yet. Just pass.')
            self.output_width = (self.input_width + self.padding_width - self.kernel_width) // self.stride + 1
            self.output_height = (self.input_height + self.padding_height - self.kernel_height) // self.stride + 1
        elif isinstance(self.module, nn.ReLU):
            print('[Front-end] ReLU is not supported yet. Just pass.')
            self.output_width = self.input_width
            self.output_height = self.input_height
        elif isinstance(self.module, nn.Dropout):
            print('[Front-end] Dropout is not supported yet. Just pass.')
            self.output_width = self.input_width
            self.output_height = self.input_height
        elif isinstance(self.module, nn.BatchNorm2d):
            print('[Front-end] BatchNorm2d is not supported yet. Just pass.')
            self.output_width = self.input_width
            self.output_height = self.input_height
        elif isinstance(self.module, nn.AdaptiveAvgPool2d):
            print('[Front-end] AdaptiveAvgPool2d is not supported yet. Just pass.')
            self.output_width = (self.input_width + self.padding_width - self.kernel_width) // self.stride + 1
            self.output_height = (self.input_height + self.padding_height - self.kernel_height) // self.stride + 1
        else:
            raise exceptions.NotSupportedException(type(self.module))

        print('[Front-end] Next layer width/height is {}/{}'.format(self.output_width, self.output_height))

        return torch.Tensor(self.batch, self.output_channel, self.output_height, self.output_width)

def print_logo():
    '''
    Print e-PlaNNer logo.
    '''

    # pylint: disable=line-too-long
    # Logo printing needs long line.

    print('                                                                                        .---. ')
    print('                                                                                       /  .  \\ ')
    print('                                                                                      |\\_/|   |')
    print('                                                                                      |   |  /|')
    print('  .-----------------------------------------------------------------------------------------\' |')
    print(' /  .-.                             ,,                                                        |')
    print('|  /   \\                \x1b[31m`7MM"""Mq.\x1b[0m`7MM           \x1b[32m`7MN.   `7MF\'`7MN.   `7MF\'\x1b[0m                   |')
    print('| |\\_.  |                 \x1b[31mMM   `MM.\x1b[0m MM             \x1b[32mMMN.    M    MMN.    M\x1b[0m                     |')
    print('|\\|  | /|   .gP"Ya        \x1b[31mMM   ,M9\x1b[0m  MM   ,6"Yb.    \x1b[32mM YMb   M    M YMb   M\x1b[0m  .gP"Ya `7Mb,od8    |')
    print('| `---\' |  ,M\'   Yb       \x1b[31mMMmmdM9\x1b[0m   MM  8)   MM    \x1b[32mM  `MN. M    M  `MN. M\x1b[0m ,M\'   Yb  MM\' \"\'    |')
    print('|   \x1b[34mC\x1b[0m   |  8M"""""" mmmmm \x1b[31mMM\x1b[0m        MM   ,pm9MM    \x1b[32mM   `MM.M    M   `MM.M\x1b[0m 8M""""""  MM        |')
    print('|       |  YM.    ,       \x1b[31mMM\x1b[0m        MM  8M   MM    \x1b[32mM     YMM    M     YMM\x1b[0m YM.    ,  MM        | *')
    print('|   \x1b[34mS\x1b[0m   |   `Mbmmd\'     \x1b[31m.JMML.\x1b[0m    .JMML.`Moo9^Yo..\x1b[32mJML.    YM  .JML.    YM\x1b[0m  `Mbmmd\'.JMML.      |')
    print('|       |                                                                                     |')
    print('|   \x1b[34mA\x1b[0m   |           execution-Planner for Neural Network efficiently running                 /')
    print('|       |-----------------------------------------------------------------------------------\'')
    print('\\   \x1b[34mP\x1b[0m   |            \x1b[34mC\x1b[0mompiling, \x1b[34mS\x1b[0mcheduling \x1b[34mA\x1b[0mnd \x1b[34mP\x1b[0mrofiling Management System')
    print(' \\     /                                Made in nprc Snu')
    print('  `---\'')
    print('')
    print('')
