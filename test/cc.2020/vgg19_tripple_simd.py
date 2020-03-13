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

hw_spec = hw.HardwareSpec(0.2, 0.8, 0.0008, 0.005, 0.64)

data_1_1 = torch.randn(1, 3, 224, 224)
conv_1_1 = nn.Conv2d(3, 64, 3, padding=1)

data_1_2 = torch.randn(1, 64, 224, 224)
conv_1_2 = nn.Conv2d(64, 64, 3, padding=1)

data_2_1 = torch.randn(1, 64, 112, 112)
conv_2_1 = nn.Conv2d(64, 128, 3, padding=1)

data_2_2 = torch.randn(1, 128, 112, 112)
conv_2_2 = nn.Conv2d(128, 128, 3, padding=1)

data_3_1 = torch.randn(1, 128, 56, 56)
conv_3_1 = nn.Conv2d(128, 256, 3, padding=1)

data_3_2 = torch.randn(1, 256, 56, 56)
conv_3_2 = nn.Conv2d(256, 256, 3, padding=1)

data_3_3 = torch.randn(1, 256, 56, 56)
conv_3_3 = nn.Conv2d(256, 256, 3, padding=1)

data_3_4 = torch.randn(1, 256, 56, 56)
conv_3_4 = nn.Conv2d(256, 256, 3, padding=1)

data_4_1 = torch.randn(1, 256, 28, 28)
conv_4_1 = nn.Conv2d(256, 512, 3, padding=1)

data_4_2 = torch.randn(1, 512, 28, 28)
conv_4_2 = nn.Conv2d(512, 512, 3, padding=1)

data_4_3 = torch.randn(1, 512, 28, 28)
conv_4_3 = nn.Conv2d(512, 512, 3, padding=1)

data_4_4 = torch.randn(1, 512, 28, 28)
conv_4_4 = nn.Conv2d(512, 512, 3, padding=1)

data_5_1 = torch.randn(1, 512, 14, 14)
conv_5_1 = nn.Conv2d(512, 512, 3, padding=1)

data_5_2 = torch.randn(1, 512, 14, 14)
conv_5_2 = nn.Conv2d(512, 512, 3, padding=1)

data_5_3 = torch.randn(1, 512, 14, 14)
conv_5_3 = nn.Conv2d(512, 512, 3, padding=1)

data_5_4 = torch.randn(1, 512, 14, 14)
conv_5_4 = nn.Conv2d(512, 512, 3, padding=1)

data_6 = torch.randn(1, 7*7*512, 1, 1)
fc_6 = nn.Linear(7*7*512, 4096)

data_7 = torch.randn(1, 4096, 1, 1)
fc_7 = nn.Linear(4096, 4096)

data_8 = torch.randn(1, 4096, 1, 1)
fc_8 = nn.Linear(4096, 1000)

pnn = pln.Planner(writeup=True, codegen=True, realsim=True)

def task(name, pnn, data, module):
    start_time = time.time()
    global hw_spec
    print('[Front-end] \x1b[32m' + str(name) + ' start...\x1b[0m\n')
    pnn.set_data(data=data, module=module, hw_spec=hw_spec, writeup_file='vgg19_simd.csv', testcase=name)
    pnn.run('../../build/tripple_simd_cnn_planner')
    print('[Front-end] \x1b[32m' + str(name) + ' finish!\x1b[0m\n')
    return time.time() - start_time

elapsed_time = 0

try:
    elapsed_time += task('conv_1_1', pnn, data_1_1, conv_1_1)
    elapsed_time += task('conv_1_2', pnn, data_1_2, conv_1_2)
    elapsed_time += task('conv_2_1', pnn, data_2_1, conv_2_1)
    elapsed_time += task('conv_2_2', pnn, data_2_2, conv_2_2)
    elapsed_time += task('conv_3_1', pnn, data_3_1, conv_3_1)
    elapsed_time += task('conv_3_2', pnn, data_3_2, conv_3_2)
    elapsed_time += task('conv_3_3', pnn, data_3_3, conv_3_3)
    elapsed_time += task('conv_3_4', pnn, data_3_4, conv_3_4)
    elapsed_time += task('conv_4_1', pnn, data_4_1, conv_4_1)
    elapsed_time += task('conv_4_2', pnn, data_4_2, conv_4_2)
    elapsed_time += task('conv_4_3', pnn, data_4_3, conv_4_3)
    elapsed_time += task('conv_4_4', pnn, data_4_4, conv_4_4)
    elapsed_time += task('conv_5_1', pnn, data_5_1, conv_5_1)
    elapsed_time += task('conv_5_2', pnn, data_5_2, conv_5_2)
    elapsed_time += task('conv_5_3', pnn, data_5_3, conv_5_3)
    elapsed_time += task('conv_5_4', pnn, data_5_4, conv_5_4)
    elapsed_time += task('fc_6', pnn, data_6, fc_6)
    elapsed_time += task('fc_7', pnn, data_7, fc_7)
    elapsed_time += task('fc_8', pnn, data_8, fc_8)
except RuntimeFail as e:
    print(e)

print('[Front-end] Elapsed time: ' + time.strftime('%M min %S sec', time.gmtime(elapsed_time)))
