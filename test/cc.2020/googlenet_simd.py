"""
Test GoogleNet Layers
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
conv_1 = nn.Conv2d(3, 64, 7, stride=2, padding=3)
#####
data_2 = torch.randn(1, 64, 56, 56)
conv_2 = nn.Conv2d(64, 64, 1)
#####
data_3 = torch.randn(1, 64, 56, 56)
conv_3 = nn.Conv2d(64, 192, 3, padding=1)
#####
data_incpt_1 = torch.randn(1, 192, 28, 28)

incpt_1_conv_1 = nn.Conv2d(192, 64, 1)

incpt_1_conv_2_1 = nn.Conv2d(192, 96, 1)
data_incpt_1_2_2 = torch.randn(1, 96, 28, 28)
incpt_1_conv_2_2 = nn.Conv2d(96, 128, 3, padding=1)

incpt_1_conv_3_1 = nn.Conv2d(192, 16, 1)
data_incpt_1_3_2 = torch.randn(1, 16, 28, 28)
incpt_1_conv_3_2 = nn.Conv2d(16, 32, 5, padding=2)

incpt_1_conv_4 = nn.Conv2d(192, 32, 1)
#####
data_incpt_2 = torch.randn(1, 256, 28, 28)

incpt_2_conv_1 = nn.Conv2d(256, 128, 1)

incpt_2_conv_2_1 = nn.Conv2d(256, 128, 1)
data_incpt_2_2_2 = torch.randn(1, 128, 28, 28)
incpt_2_conv_2_2 = nn.Conv2d(128, 192, 3, padding=1)

incpt_2_conv_3_1 = nn.Conv2d(256, 32, 1)
data_incpt_2_3_2 = torch.randn(1, 32, 28, 28)
incpt_2_conv_3_2 = nn.Conv2d(32, 96, 5, padding=2)

incpt_2_conv_4 = nn.Conv2d(256, 64, 1)
#####
data_incpt_3 = torch.randn(1, 480, 14, 14)

incpt_3_conv_1 = nn.Conv2d(480, 192, 1)

incpt_3_conv_2_1 = nn.Conv2d(480, 96, 1)
data_incpt_3_2_2 = torch.randn(1, 96, 14, 14)
incpt_3_conv_2_2 = nn.Conv2d(96, 208, 3, padding=1)

incpt_3_conv_3_1 = nn.Conv2d(480, 16, 1)
data_incpt_3_3_2 = torch.randn(1, 16, 14, 14)
incpt_3_conv_3_2 = nn.Conv2d(16, 48, 5, padding=2)

incpt_3_conv_4 = nn.Conv2d(480, 64, 1)
#####
data_incpt_4 = torch.randn(1, 512, 14, 14)

incpt_4_conv_1 = nn.Conv2d(512, 160, 1)

incpt_4_conv_2_1 = nn.Conv2d(512, 112, 1)
data_incpt_4_2_2 = torch.randn(1, 112, 14, 14)
incpt_4_conv_2_2 = nn.Conv2d(112, 224, 3, padding=1)

incpt_4_conv_3_1 = nn.Conv2d(512, 24, 1)
data_incpt_4_3_2 = torch.randn(1, 24, 14, 14)
incpt_4_conv_3_2 = nn.Conv2d(24, 64, 5, padding=2)

incpt_4_conv_4 = nn.Conv2d(512, 64, 1)
#####
data_incpt_5 = torch.randn(1, 512, 14, 14)

incpt_5_conv_1 = nn.Conv2d(512, 128, 1)

incpt_5_conv_2_1 = nn.Conv2d(512, 128, 1)
data_incpt_5_2_2 = torch.randn(1, 128, 14, 14)
incpt_5_conv_2_2 = nn.Conv2d(128, 256, 3, padding=1)

incpt_5_conv_3_1 = nn.Conv2d(512, 24, 1)
data_incpt_5_3_2 = torch.randn(1, 24, 14, 14)
incpt_5_conv_3_2 = nn.Conv2d(24, 64, 5, padding=2)

incpt_5_conv_4 = nn.Conv2d(512, 64, 1)
#####
data_incpt_6 = torch.randn(1, 512, 14, 14)

incpt_6_conv_1 = nn.Conv2d(512, 112, 1)

incpt_6_conv_2_1 = nn.Conv2d(512, 144, 1)
data_incpt_6_2_2 = torch.randn(1, 144, 14, 14)
incpt_6_conv_2_2 = nn.Conv2d(144, 288, 3, padding=1)

incpt_6_conv_3_1 = nn.Conv2d(512, 32, 1)
data_incpt_6_3_2 = torch.randn(1, 32, 14, 14)
incpt_6_conv_3_2 = nn.Conv2d(32, 64, 5, padding=2)

incpt_6_conv_4 = nn.Conv2d(512, 64, 1)
#####
data_incpt_7 = torch.randn(1, 528, 14, 14)

incpt_7_conv_1 = nn.Conv2d(528, 256, 1)

incpt_7_conv_2_1 = nn.Conv2d(528, 160, 1)
data_incpt_7_2_2 = torch.randn(1, 160, 14, 14)
incpt_7_conv_2_2 = nn.Conv2d(160, 320, 3, padding=1)

incpt_7_conv_3_1 = nn.Conv2d(528, 32, 1)
data_incpt_7_3_2 = torch.randn(1, 32, 14, 14)
incpt_7_conv_3_2 = nn.Conv2d(32, 128, 5, padding=2)

incpt_7_conv_4 = nn.Conv2d(528, 128, 1)
#####
data_incpt_8 = torch.randn(1, 832, 7, 7)

incpt_8_conv_1 = nn.Conv2d(832, 256, 1)

incpt_8_conv_2_1 = nn.Conv2d(832, 160, 1)
data_incpt_8_2_2 = torch.randn(1, 160, 7, 7)
incpt_8_conv_2_2 = nn.Conv2d(160, 320, 3, padding=1)

incpt_8_conv_3_1 = nn.Conv2d(832, 32, 1)
data_incpt_8_3_2 = torch.randn(1, 32, 7, 7)
incpt_8_conv_3_2 = nn.Conv2d(32, 128, 5, padding=2)

incpt_8_conv_4 = nn.Conv2d(832, 128, 1)
#####
data_incpt_9 = torch.randn(1, 832, 7, 7)

incpt_9_conv_1 = nn.Conv2d(832, 384, 1)

incpt_9_conv_2_1 = nn.Conv2d(832, 192, 1)
data_incpt_9_2_2 = torch.randn(1, 192, 7, 7)
incpt_9_conv_2_2 = nn.Conv2d(192, 384, 3, padding=1)

incpt_9_conv_3_1 = nn.Conv2d(832, 48, 1)
data_incpt_9_3_2 = torch.randn(1, 48, 7, 7)
incpt_9_conv_3_2 = nn.Conv2d(48, 128, 5, padding=2)

incpt_9_conv_4 = nn.Conv2d(832, 128, 1)
#####
data_10 = torch.randn(1, 1024, 1, 1)
fc_1 = nn.Linear(1024, 1000)

pnn = pln.Planner(writeup=True, codegen=True, realsim=True)

def task(name, pnn, data, module):
    global hw_spec
    print('[Front-end] \x1b[32m' + str(name) + ' start...\x1b[0m\n')
    pnn.set_data(data=data, module=module, hw_spec=hw_spec, writeup_file='vgg19_simd.csv', testcase=name)
    pnn.run('../../build/cnn_planner')
    print('[Front-end] \x1b[32m' + str(name) + ' finish!\x1b[0m\n')

start_time = time.time()
elapsed_time = 0

try:
    task('conv_1', pnn, data_1, conv_1)
    task('conv_2', pnn, data_2, conv_2)
    task('conv_3', pnn, data_3, conv_3)

    task('incpt_1_conv_1', pnn, data_incpt_1, incpt_1_conv_1)
    task('incpt_1_conv_2_1', pnn, data_incpt_1, incpt_1_conv_2_1)
    task('incpt_1_conv_2_2', pnn, data_incpt_1_2_2, incpt_1_conv_2_2)
    task('incpt_1_conv_3_1', pnn, data_incpt_1, incpt_1_conv_3_1)
    task('incpt_1_conv_3_2', pnn, data_incpt_1_3_2, incpt_1_conv_3_2)
    task('incpt_1_conv_4', pnn, data_incpt_1, incpt_1_conv_4)

    task('incpt_2_conv_1', pnn, data_incpt_2, incpt_2_conv_1)
    task('incpt_2_conv_2_1', pnn, data_incpt_2, incpt_2_conv_2_1)
    task('incpt_2_conv_2_2', pnn, data_incpt_2_2_2, incpt_2_conv_2_2)
    task('incpt_2_conv_3_1', pnn, data_incpt_2, incpt_2_conv_3_1)
    task('incpt_2_conv_3_2', pnn, data_incpt_2_3_2, incpt_2_conv_3_2)
    task('incpt_2_conv_4', pnn, data_incpt_2, incpt_2_conv_4)

    task('incpt_3_conv_1', pnn, data_incpt_3, incpt_3_conv_1)
    task('incpt_3_conv_2_1', pnn, data_incpt_3, incpt_3_conv_2_1)
    task('incpt_3_conv_2_2', pnn, data_incpt_3_2_2, incpt_3_conv_2_2)
    task('incpt_3_conv_3_1', pnn, data_incpt_3, incpt_3_conv_3_1)
    task('incpt_3_conv_3_2', pnn, data_incpt_3_3_2, incpt_3_conv_3_2)
    task('incpt_3_conv_4', pnn, data_incpt_3, incpt_3_conv_4)

    task('incpt_4_conv_1', pnn, data_incpt_4, incpt_4_conv_1)
    task('incpt_4_conv_2_1', pnn, data_incpt_4, incpt_4_conv_2_1)
    task('incpt_4_conv_2_2', pnn, data_incpt_4_2_2, incpt_4_conv_2_2)
    task('incpt_4_conv_3_1', pnn, data_incpt_4, incpt_4_conv_3_1)
    task('incpt_4_conv_3_2', pnn, data_incpt_4_3_2, incpt_4_conv_3_2)
    task('incpt_4_conv_4', pnn, data_incpt_4, incpt_4_conv_4)

    task('incpt_5_conv_1', pnn, data_incpt_5, incpt_5_conv_1)
    task('incpt_5_conv_2_1', pnn, data_incpt_5, incpt_5_conv_2_1)
    task('incpt_5_conv_2_2', pnn, data_incpt_5_2_2, incpt_5_conv_2_2)
    task('incpt_5_conv_3_1', pnn, data_incpt_5, incpt_5_conv_3_1)
    task('incpt_5_conv_3_2', pnn, data_incpt_5_3_2, incpt_5_conv_3_2)
    task('incpt_5_conv_4', pnn, data_incpt_5, incpt_5_conv_4)

    task('incpt_6_conv_1', pnn, data_incpt_6, incpt_6_conv_1)
    task('incpt_6_conv_2_1', pnn, data_incpt_6, incpt_6_conv_2_1)
    task('incpt_6_conv_2_2', pnn, data_incpt_6_2_2, incpt_6_conv_2_2)
    task('incpt_6_conv_3_1', pnn, data_incpt_6, incpt_6_conv_3_1)
    task('incpt_6_conv_3_2', pnn, data_incpt_6_3_2, incpt_6_conv_3_2)
    task('incpt_6_conv_4', pnn, data_incpt_6, incpt_6_conv_4)

    task('incpt_7_conv_1', pnn, data_incpt_7, incpt_7_conv_1)
    task('incpt_7_conv_2_1', pnn, data_incpt_7, incpt_7_conv_2_1)
    task('incpt_7_conv_2_2', pnn, data_incpt_7_2_2, incpt_7_conv_2_2)
    task('incpt_7_conv_3_1', pnn, data_incpt_7, incpt_7_conv_3_1)
    task('incpt_7_conv_3_2', pnn, data_incpt_7_3_2, incpt_7_conv_3_2)
    task('incpt_7_conv_4', pnn, data_incpt_7, incpt_7_conv_4)

    task('incpt_8_conv_1', pnn, data_incpt_8, incpt_8_conv_1)
    task('incpt_8_conv_2_1', pnn, data_incpt_8, incpt_8_conv_2_1)
    task('incpt_8_conv_2_2', pnn, data_incpt_8_2_2, incpt_8_conv_2_2)
    task('incpt_8_conv_3_1', pnn, data_incpt_8, incpt_8_conv_3_1)
    task('incpt_8_conv_3_2', pnn, data_incpt_8_3_2, incpt_8_conv_3_2)
    task('incpt_8_conv_4', pnn, data_incpt_8, incpt_8_conv_4)

    task('incpt_9_conv_1', pnn, data_incpt_9, incpt_9_conv_1)
    task('incpt_9_conv_2_1', pnn, data_incpt_9, incpt_9_conv_2_1)
    task('incpt_9_conv_2_2', pnn, data_incpt_9_2_2, incpt_9_conv_2_2)
    task('incpt_9_conv_3_1', pnn, data_incpt_9, incpt_9_conv_3_1)
    task('incpt_9_conv_3_2', pnn, data_incpt_9_3_2, incpt_9_conv_3_2)
    task('incpt_9_conv_4', pnn, data_incpt_9, incpt_9_conv_4)

    task('fc_1', pnn, data_10, fc_1)
    elapsed_time = time.time() - start_time
except RuntimeFail as e:
    print(e)

print('[Front-end] Elapsed time: ' + time.strftime('%M min %S sec', time.gmtime(elapsed_time)))
