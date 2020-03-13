"""
Test AlexNet convolutional layers
"""
import sys
sys.path.insert(0, '../../python/planner')

import planner as pln
import hardware as hw
import nn

import torch

import time

simd_cfg_path = '../../hwcfg/simd.json'
hw_spec = hw.HardwareSpec(simd_cfg_path)
#systolic_cfg_path = '../../hwcfg/systolic.json'
#hw_spec = hw.HardwareSpec(systolic_cfg_path)

data_1 = torch.randn(1, 3, 227, 227)
conv_1 = nn.Conv2d(3, 96, 11, stride=4)

data_2 = torch.randn(1, 96, 27, 27)
conv_2 = nn.Conv2d(96, 256, 5, stride=1, padding=2)

data_3 = torch.randn(1, 256, 13, 13)
conv_3 = nn.Conv2d(256, 384, 3, stride=1, padding=1)

data_4 = torch.randn(1, 384, 13, 13)
conv_4 = nn.Conv2d(384, 384, 3, stride=1, padding=1)

data_5 = torch.randn(1, 384, 13, 13)
conv_5 = nn.Conv2d(384, 256, 3, stride=1, padding=1)

data_6 = torch.randn(1, 9216, 1, 1)
fc_1 = nn.Linear(9216, 4096)

data_7 = torch.randn(1, 4096, 1, 1)
fc_2 = nn.Linear(4096, 4096)

data_8 = torch.randn(1, 4096, 1, 1)
fc_3 = nn.Linear(4096, 1000)

pnn = pln.Planner(writeup=True, codegen=True, realsim=True)

def task(name, pnn, data, module):
    global hw_spec
    print('[Front-end] \x1b[32m' + str(name) + ' start...\x1b[0m\n')
    pnn.set_data(data=data, module=module, hw_spec=hw_spec, writeup_file='alexnet.csv', testcase=name)
    pnn.run('../../build/cnn_planner')
    print('[Front-end] \x1b[32m' + str(name) + ' finish!\x1b[0m\n')

start_time = time.time()
elapsed_time = 0
try:
    task('conv_1', pnn, data_1, conv_1)
    task('conv_2', pnn, data_2, conv_2)
    task('conv_3', pnn, data_3, conv_3)
    task('conv_4', pnn, data_4, conv_4)
    task('conv_5', pnn, data_5, conv_5)

    task('fc_1', pnn, data_6, fc_1)
    task('fc_2', pnn, data_7, fc_2)
    task('fc_3', pnn, data_8, fc_3)
    elapsed_time = time.time() - start_time
except RuntimeFail as e:
    print(e)

print('[Front-end] Elapsed time: ' + time.strftime('%H hours %M min %S sec', time.gmtime(elapsed_time)))
