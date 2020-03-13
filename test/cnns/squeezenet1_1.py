r"""
SqueezeNet v1.1 example
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

data = torch.Tensor(1, 3, 227, 227);
sqznet1_1 = models.squeezenet1_1()

pnn = pln.Planner()

start_time = time.time()

e_data_1 = None
e_data_3 = None

for name, module in sqznet1_1.named_modules():
  if isinstance(module, torch.nn.Sequential):
    continue

  pnn.set_data(data=data, module=module, hw_spec=hw_spec, layer_name=name)

  if name[:3] == 'exp' and name[-1] == '1':
    e_data_1 = pnn.run('../../build')
  elif name[:3] == 'exp' and name[-1] == '3':
    e_data_3 = pnn.run('../../build')
    data = torch.cat((e_data_1, e_data_3), 1)
  else:
    data = pnn.run('../../build')

elapsed_time = time.time() - start_time
print('[Front-end] Elapsed time: ' + time.strftime('%H hours %M min %S sec', time.gmtime(elapsed_time)))
