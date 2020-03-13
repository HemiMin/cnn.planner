"""
Test VGG19 Layers
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

data_1 = torch.randn(1, 3, 224, 224)
conv_1 = nn.Conv2d(3, 96, 7, padding=2)

data_2_1 = torch.randn(1, 96, 55, 55)
conv_2_1 = nn.Conv2d(96, 16, 1, padding=0)

data_2_2 = torch.randn(1, 16, 55, 55)
conv_2_2 = nn.Conv2d(16, 64, 1, padding=0)
conv_2_3 = nn.Conv2d(16, 64, 3, padding=1)

data_3_1 = torch.randn(1, 128, 55, 55)
conv_3_1 = nn.Conv2d(128, 16, 1, padding=0)

data_3_2 = torch.randn(1, 32, 55, 55)
conv_3_2 = nn.Conv2d(32, 128, 1, padding=0)
conv_3_3 = nn.Conv2d(32, 128, 3, padding=1)

data_4_1 = torch.randn(1, 256, 27, 27)
conv_4_1 = nn.Conv2d(256, 32, 1, padding=0)

data_4_2 = torch.randn(1, 32, 27, 27)
conv_4_2 = nn.Conv2d(32, 128, 1, padding=0)
conv_4_3 = nn.Conv2d(32, 128, 3, padding=1)

data_5_1 = torch.randn(1, 256, 27, 27)
conv_5_1 = nn.Conv2d(256, 48, 1, padding=0)

data_5_2 = torch.randn(1, 48, 27, 27)
conv_5_2 = nn.Conv2d(48, 192, 1, padding=0)
conv_5_3 = nn.Conv2d(48, 192, 3, padding=1)

data_6_1 = torch.randn(1, 384, 27, 27)
conv_6_1 = nn.Conv2d(384, 48, 1, padding=0)

data_6_2 = torch.randn(1, 48, 27, 27)
conv_6_2 = nn.Conv2d(48, 192, 1, padding=0)
conv_6_3 = nn.Conv2d(48, 192, 3, padding=1)

data_7_1 = torch.randn(1, 384, 27, 27)
conv_7_1 = nn.Conv2d(384, 64, 1, padding=0)

data_7_2 = torch.randn(1, 64, 27, 27)
conv_7_2 = nn.Conv2d(64, 256, 1, padding=0)
conv_7_3 = nn.Conv2d(64, 256, 3, padding=1)

data_8_1 = torch.randn(1, 512, 13, 13)
conv_8_1 = nn.Conv2d(512, 64, 1, padding=0)

data_8_2 = torch.randn(1, 64, 13, 13)
conv_8_2 = nn.Conv2d(64, 256, 1, padding=0)
conv_8_3 = nn.Conv2d(64, 256, 3, padding=1)

data_9 = torch.randn(1, 512, 13, 13)
conv_9 = nn.Conv2d(512, 1000, 1, padding=0)

pnn = pln.Planner(writeup=True, codegen=True, realsim=True)

def task(name, pnn, data, module):
    global hw_spec
    print('[Front-end] \x1b[32m' + str(name) + ' start...\x1b[0m\n')
    pnn.set_data(data=data, module=module, hw_spec=hw_spec, writeup_file='vgg19.csv', testcase=name)
    pnn.run('../../build/cnn_planner')
    print('[Front-end] \x1b[32m' + str(name) + ' finish!\x1b[0m\n')

start_time = time.time()
elapsed_time = 0

try:
    task('conv_1', pnn, data_1, conv_1)
    task('conv_2_1', pnn, data_2_1, conv_2_1)
    task('conv_2_2', pnn, data_2_2, conv_2_2)
    task('conv_2_3', pnn, data_2_2, conv_2_3)
    task('conv_3_1', pnn, data_3_1, conv_3_1)
    task('conv_3_2', pnn, data_3_2, conv_3_2)
    task('conv_3_3', pnn, data_3_2, conv_3_3)
    task('conv_4_1', pnn, data_4_1, conv_4_1)
    task('conv_4_2', pnn, data_4_2, conv_4_2)
    task('conv_4_3', pnn, data_4_2, conv_4_3)
    task('conv_5_1', pnn, data_5_1, conv_5_1)
    task('conv_5_2', pnn, data_5_2, conv_5_2)
    task('conv_5_3', pnn, data_5_2, conv_5_3)
    task('conv_6_1', pnn, data_6_1, conv_6_1)
    task('conv_6_2', pnn, data_6_2, conv_6_2)
    task('conv_6_3', pnn, data_6_2, conv_6_3)
    task('conv_7_1', pnn, data_7_1, conv_7_1)
    task('conv_7_2', pnn, data_7_2, conv_7_2)
    task('conv_7_3', pnn, data_7_2, conv_7_3)
    task('conv_8_1', pnn, data_8_1, conv_8_1)
    task('conv_8_2', pnn, data_8_2, conv_8_2)
    task('conv_8_3', pnn, data_8_2, conv_8_3)
    task('conv_9', pnn, data_9, conv_9)
    elapsed_time = time.time() - start_time
except RuntimeFail as e:
    print(e)

print('[Front-end] Elapsed time: ' + time.strftime('%H hours %M min %S sec', time.gmtime(elapsed_time)))
