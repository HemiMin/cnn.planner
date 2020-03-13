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
conv_1 = nn.Conv2d(3, 32, 3, padding=1)

data_2 = torch.randn(1, 1, 112, 112)
conv_2 = nn.Conv2d(1, 1, 3, padding=1)

data_3 = torch.randn(1, 32, 112, 112)
conv_3 = nn.Conv2d(32, 64, 1, padding=0)

data_4 = torch.randn(1, 1, 112, 112)
conv_4 = nn.Conv2d(1, 1, 3, padding=1)

data_5 = torch.randn(1, 64, 56, 56)
conv_5 = nn.Conv2d(64, 128, 1, padding=0)

data_6 = torch.randn(1, 1, 56, 56)
conv_6 = nn.Conv2d(1, 1, 3, padding=1)

data_7 = torch.randn(1, 128, 56, 56)
conv_7 = nn.Conv2d(128, 128, 1, padding=0)

data_8 = torch.randn(1, 1, 56, 56)
conv_8 = nn.Conv2d(1, 1, 3, padding=1)

data_9 = torch.randn(1, 128, 28, 28)
conv_9 = nn.Conv2d(128, 256, 1, padding=0)

data_10 = torch.randn(1, 1, 28, 28)
conv_10 = nn.Conv2d(1, 1, 3, padding=1)

data_11 = torch.randn(1, 256, 28, 28)
conv_11 = nn.Conv2d(256, 256, 1, padding=0)

data_12 = torch.randn(1, 1, 28, 28)
conv_12 = nn.Conv2d(1, 1, 3, padding=1)

data_13 = torch.randn(1, 256, 14, 14)
conv_13 = nn.Conv2d(256, 512, 1, padding=0)

data_14 = torch.randn(1, 1, 14, 14)
conv_14 = nn.Conv2d(1, 1, 3, padding=1)

data_15 = torch.randn(1, 512, 14, 14)
conv_15 = nn.Conv2d(512, 512, 1, padding=0)

data_16 = torch.randn(1, 1, 14, 14)
conv_16 = nn.Conv2d(1, 1, 3, padding=1)

data_17 = torch.randn(1, 512, 14, 14)
conv_17 = nn.Conv2d(512, 512, 1, padding=0)

data_18 = torch.randn(1, 1, 14, 14)
conv_18 = nn.Conv2d(1, 1, 3, padding=1)

data_19 = torch.randn(1, 512, 14, 14)
conv_19 = nn.Conv2d(512, 512, 1, padding=0)

data_20 = torch.randn(1, 1, 14, 14)
conv_20 = nn.Conv2d(1, 1, 3, padding=1)

data_21 = torch.randn(1, 512, 14, 14)
conv_21 = nn.Conv2d(512, 512, 1, padding=0)

data_22 = torch.randn(1, 1, 14, 14)
conv_22 = nn.Conv2d(1, 1, 3, padding=1)

data_23 = torch.randn(1, 512, 14, 14)
conv_23 = nn.Conv2d(512, 512, 1, padding=0)

data_24 = torch.randn(1, 1, 14, 14)
conv_24 = nn.Conv2d(1, 1, 3, padding=1)

data_25 = torch.randn(1, 512, 7, 7)
conv_25 = nn.Conv2d(512, 1024, 1, padding=0)

data_26 = torch.randn(1, 1, 7, 7)
conv_26 = nn.Conv2d(1, 1, 3, padding=1)

data_27 = torch.randn(1, 1024, 7, 7)
conv_27 = nn.Conv2d(1024, 1024, 1, padding=0)

data_28 = torch.randn(1, 1024, 1, 1)
fc_1 = nn.Linear(1024, 1000)

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
    task('conv_2', pnn, data_2, conv_2)
    task('conv_3', pnn, data_3, conv_3)
    task('conv_4', pnn, data_4, conv_4)
    task('conv_5', pnn, data_5, conv_5)
    task('conv_6', pnn, data_6, conv_6)
    task('conv_7', pnn, data_7, conv_7)
    task('conv_8', pnn, data_8, conv_8)
    task('conv_9', pnn, data_9, conv_9)
    task('conv_10', pnn, data_10, conv_10)
    task('conv_11', pnn, data_11, conv_11)
    task('conv_12', pnn, data_12, conv_12)
    task('conv_13', pnn, data_13, conv_13)
    task('conv_14', pnn, data_14, conv_14)
    task('conv_15', pnn, data_15, conv_15)
    task('conv_16', pnn, data_16, conv_16)
    task('conv_17', pnn, data_17, conv_17)
    task('conv_18', pnn, data_18, conv_18)
    task('conv_19', pnn, data_19, conv_19)
    task('conv_20', pnn, data_20, conv_20)
    task('conv_21', pnn, data_21, conv_21)
    task('conv_22', pnn, data_22, conv_22)
    task('conv_23', pnn, data_23, conv_23)
    task('conv_24', pnn, data_24, conv_24)
    task('conv_25', pnn, data_25, conv_25)
    task('conv_26', pnn, data_26, conv_26)
    task('conv_27', pnn, data_27, conv_27)
    task('fc_1', pnn, data_28, fc_1)
    elapsed_time = time.time() - start_time
except RuntimeFail as e:
    print(e)

print('[Front-end] Elapsed time: ' + time.strftime('%H hours %M min %S sec', time.gmtime(elapsed_time)))
