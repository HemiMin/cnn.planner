"""
Test VGG16 Layers
"""
import sys
sys.path.insert(0, '../../python/planner')

import planner as pln
import hardware as hw
import nn

import torch

import time

hw_spec = hw.HardwareSpec(0.2, 0.8, 0.0008, 0.005, 0.64)

data_1 = torch.randn(1, 3, 224, 224)
conv_1 = nn.Conv2d(3, 64, 3, stride=1, padding=1)

data_2 = torch.randn(1, 64, 224, 224)
conv_2 = nn.Conv2d(64, 64, 3, stride=1, padding=1)

data_3 = torch.randn(1, 64, 112, 112)
conv_3 = nn.Conv2d(64, 128, 3, stride=1, padding=1)

data_4 = torch.randn(1, 128, 112, 112)
conv_4 = nn.Conv2d(128, 128, 3, stride=1, padding=1)

data_5 = torch.randn(1, 128, 56, 56)
conv_5 = nn.Conv2d(128, 256, 3, stride=1, padding=1)

data_6 = torch.randn(1, 256, 56, 56)
conv_6 = nn.Conv2d(256, 256, 3, stride=1, padding=1)

data_7 = torch.randn(1, 256, 56, 56)
conv_7 = nn.Conv2d(256, 256, 3, stride=1, padding=1)

data_8 = torch.randn(1, 256, 28, 28)
conv_8 = nn.Conv2d(256, 512, 3, stride=1, padding=1)

data_9 = torch.randn(1, 512, 28, 28)
conv_9 = nn.Conv2d(512, 512, 3, stride=1, padding=1)

data_10 = torch.randn(1, 512, 28, 28)
conv_10 = nn.Conv2d(512, 512, 3, stride=1, padding=1)

data_11 = torch.randn(1, 512, 14, 14)
conv_11 = nn.Conv2d(512, 512, 3, stride=1, padding=1)

data_12 = torch.randn(1, 512, 14, 14)
conv_12 = nn.Conv2d(512, 512, 3, stride=1, padding=1)

data_13 = torch.randn(1, 512, 14, 14)
conv_13 = nn.Conv2d(512, 512, 3, stride=1, padding=1)

data_14 = torch.randn(1, 7*7*512, 1, 1)
fc_1 = nn.Linear(7*7*512, 4096)

data_15 = torch.randn(1, 4096, 1, 1)
fc_2 = nn.Linear(4096, 4096)

data_16 = torch.randn(1, 4096, 1, 1)
fc_3 = nn.Linear(4096, 1000)

pnn = pln.Planner(writeup=True, codegen=True, realsim=True)

def task(name, pnn, data, module):
    start_time = time.time()
    global hw_spec
    print('[Front-end] \x1b[32m' + str(name) + ' start...\x1b[0m\n')
    pnn.set_data(data=data, module=module, hw_spec=hw_spec, writeup_file='vgg16.csv', testcase=name)
    pnn.run('../../build/simd_cnn_planner')
    print('[Front-end] \x1b[32m' + str(name) + ' finish!\x1b[0m\n')
    return time.time() - start_time

elapsed_time = 0
try:
    elapsed_time += task('conv_1', pnn, data_1, conv_1)
    elapsed_time += task('conv_2', pnn, data_2, conv_2)
    elapsed_time += task('conv_3', pnn, data_3, conv_3)
    elapsed_time += task('conv_4', pnn, data_4, conv_4)
    elapsed_time += task('conv_5', pnn, data_5, conv_5)
    elapsed_time += task('conv_6', pnn, data_6, conv_6)
    elapsed_time += task('conv_7', pnn, data_7, conv_7)
    elapsed_time += task('conv_8', pnn, data_8, conv_8)
    elapsed_time += task('conv_9', pnn, data_9, conv_9)
    elapsed_time += task('conv_10', pnn, data_10, conv_10)
    elapsed_time += task('conv_11', pnn, data_11, conv_11)
    elapsed_time += task('conv_12', pnn, data_12, conv_12)
    elapsed_time += task('conv_13', pnn, data_13, conv_13)

    elapsed_time += task('fc_1', pnn, data_14, fc_1)
    elapsed_time += task('fc_2', pnn, data_15, fc_2)
    elapsed_time += task('fc_3', pnn, data_16, fc_3)
except RuntimeFail as e:
    print(e)

print('[Front-end] Elapsed time: ' + time.strftime('%M min %S sec', time.gmtime(elapsed_time)))
