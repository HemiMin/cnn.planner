r"""
ResNet-50 example
"""
import sys
sys.path.insert(0, '../../python')

import planner as pln
import hardware as hw
import dataset
import models

import torch.nn
import torch

import time

simd_cfg_path = '../../hwcfg/simd.json'
hw_spec = hw.HardwareSpec(simd_cfg_path)

data = torch.Tensor(1, 3, 256, 256);
resnet50 = models.resnet50()

pnn = pln.Planner()

start_time = time.time()

for name, module in resnet50.named_modules():
  if isinstance(module, torch.nn.Sequential):
    continue
  pnn.set_data(data=data, module=module, hw_spec=hw_spec, layer_name=name)
  data = pnn.run('../../build')

elapsed_time = time.time() - start_time
print('[Front-end] Elapsed time: ' + time.strftime('%H hours %M min %S sec', time.gmtime(elapsed_time)))
