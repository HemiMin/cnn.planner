r"""
ImageNet dataset batch*224*224*3
"""
import torch

def imagenet(batch=1):
    r'''
    Return the dataset instance includes imagenet dimension information.
    '''
    return torch.Tensor(batch, 3, 224, 224)

def darknet(batch=1):
  r'''
  Return the dataset instance includes darknet (3x256x256) dimension information.
  '''
  return torch.Tensor(batch, 3, 256, 256)

def coco(batch=1):
  r'''
  Return the dataset instance includes coco dimension information.
  '''
  return torch.Tensor(batch, 3, 608, 608)
