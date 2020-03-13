r"""
YOLOv2 example 
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

data = dataset.coco()
yolov2 = models.yolov2()

pnn = pln.Planner()

start_time = time.time()

conv_5_5 = None
conv_6_7 = None
conv_7 = None
for name, module in yolov2.named_modules():
  if isinstance(module, torch.nn.Sequential):
    continue
  if name == 'conv_7':
    data = conv_5_5
  elif name == 'conv_8':
    data = torch.cat([conv_7, conv_6_7], dim=1)
  pnn.set_data(data=data, module=module, hw_spec=hw_spec, layer_name=name)
  data = pnn.run('../../build')
  if name == 'conv_5_5':
    conv_5_5 = data
  if name == 'conv_6_7':
    conv_6_7 = data
  if name == 'conv_7':
    reorg_stride = 2
    data =  torch.Tensor(data.shape[0], 
                         data.shape[1] * reorg_stride * reorg_stride, 
                         data.shape[2] // reorg_stride,
                         data.shape[3] // reorg_stride)
    conv_7 = data

elapsed_time = time.time() - start_time
print('[Front-end] Elapsed time: ' + time.strftime('%H hours %M min %S sec', time.gmtime(elapsed_time)))
