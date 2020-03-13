import sys
sys.path.insert(0, '../../python')

import planner as pln
import hardware as hw
import dataset
import models

import torch.nn
import torch

import time

x86_cfg_path = '../../hwcfg/x86.json'
hw_spec = hw.HardwareSpec(x86_cfg_path)

data = dataset.darknet()
darknet19 = models.darknet19()

pnn = pln.Planner()

start_time = time.time()

for name, module in darknet19.named_modules():
  if isinstance(module, torch.nn.Sequential):
    continue
  pnn.set_data(data=data, module=module, hw_spec=hw_spec, layer_name=name)
  data = pnn.run('../../build')

elapsed_time = time.time() - start_time
print('[Front-end] Elapsed time: ' + time.strftime('%H hours %M min %S sec', time.gmtime(elapsed_time)))
