r"""
Preprocessing class for AlexNet
"""
import exceptions
import torchvision
import torch.nn as nn

def alexnet():
    r'''
    Return AlexNet layers which can be supported.
    '''
    conv_layer = ['conv_1', 'conv_2', 'conv_3', 'conv_4', 'conv_5']
    pool_layer = ['pool_1', 'pool_2', 'pool_3']
    fc_layer = ['fc_1', 'fc_2', 'fc_3']

    tv_alexnet = torchvision.models.alexnet(pretrained=False, progress=True)

    tv_alexnet.features[0].padding = (0, 0)
    tv_alexnet.features[0].out_channels = 96
    tv_alexnet.features[3].in_channels = 96
    tv_alexnet.features[3].out_channels = 256
    tv_alexnet.features[6].in_channels = 256
    tv_alexnet.features[8].out_channels = 384
    tv_alexnet.features[10].in_channels = 384

    layers = nn.Sequential()

    conv_layer.reverse()
    pool_layer.reverse()
    fc_layer.reverse()

    # Feature Extraction Layers
    for feature in tv_alexnet.features:
        if isinstance(feature, nn.Conv2d):
            layers.add_module(conv_layer.pop(), feature)
        elif isinstance(feature, nn.ReLU):
            layers.add_module('ReLU', feature)
        elif isinstance(feature, nn.MaxPool2d):
            layers.add_module(pool_layer.pop(), feature)
        else:
            raise exceptions.NotSupportedException(type(feature))

    # Classifier Layers
    for classify in tv_alexnet.classifier:
        if isinstance(classify, nn.Linear):
            layers.add_module(fc_layer.pop(), classify)
        elif isinstance(classify, nn.Dropout):
            layers.add_module('Dropout', classify)
        elif isinstance(classify, nn.ReLU):
            layers.add_module('ReLU', classify)
        else:
            raise exceptions.NotSupportedException(type(classify))

    return layers

def vgg16():
    r'''
    Return Vgg16 layers which can be supported.
    '''
    conv_layer = ['conv_1_1', 'conv_1_2',
                  'conv_2_1', 'conv_2_2',
                  'conv_3_1', 'conv_3_2', 'conv_3_3',
                  'conv_4_1', 'conv_4_2', 'conv_4_3',
                  'conv_5_1', 'conv_5_2', 'conv_5_3']
    pool_layer = ['pool_1', 'pool_2', 'pool_3', 'pool_4', 'pool_5']
    fc_layer = ['fc_1', 'fc_2', 'fc_3']

    tv_vgg16 = torchvision.models.vgg16(pretrained=False, progress=True)
    layers = nn.Sequential()

    conv_layer.reverse()
    pool_layer.reverse()
    fc_layer.reverse()

    # Feature Extraction Layers
    for feature in tv_vgg16.features:
        if isinstance(feature, nn.Conv2d):
            layers.add_module(conv_layer.pop(), feature)
        elif isinstance(feature, nn.ReLU):
            layers.add_module('ReLU', feature)
        elif isinstance(feature, nn.MaxPool2d):
            layers.add_module(pool_layer.pop(), feature)
        else:
            raise exceptions.NotSupportedException(type(feature))

    # Classifier Layers
    for classify in tv_vgg16.classifier:
        if isinstance(classify, nn.Linear):
            layers.add_module(fc_layer.pop(), classify)
        elif isinstance(classify, nn.Dropout):
            layers.add_module('Dropout', classify)
        elif isinstance(classify, nn.ReLU):
            layers.add_module('ReLU', classify)
        else:
            raise exceptions.NotSupportedException(type(classify))

    return layers

def vgg19():
    r'''
    Return Vgg19 layers which can be supported.
    '''
    conv_layer = ['conv_1_1', 'conv_1_2',
                  'conv_2_1', 'conv_2_2',
                  'conv_3_1', 'conv_3_2', 'conv_3_3', 'conv_3_4',
                  'conv_4_1', 'conv_4_2', 'conv_4_3', 'conv_4_4',
                  'conv_5_1', 'conv_5_2', 'conv_5_3', 'conv_5_4']
    pool_layer = ['pool_1', 'pool_2', 'pool_3', 'pool_4', 'pool_5']
    fc_layer = ['fc_1', 'fc_2', 'fc_3']

    tv_vgg19 = torchvision.models.vgg19(pretrained=False, progress=True)
    layers = nn.Sequential()

    conv_layer.reverse()
    pool_layer.reverse()
    fc_layer.reverse()

    # Feature Extraction Layers
    for feature in tv_vgg19.features:
        if isinstance(feature, nn.Conv2d):
            layers.add_module(conv_layer.pop(), feature)
        elif isinstance(feature, nn.ReLU):
            layers.add_module('ReLU', feature)
        elif isinstance(feature, nn.MaxPool2d):
            layers.add_module(pool_layer.pop(), feature)
        else:
            raise exceptions.NotSupportedException(type(feature))

    # Classifier Layers
    for classify in tv_vgg19.classifier:
        if isinstance(classify, nn.Linear):
            layers.add_module(fc_layer.pop(), classify)
        elif isinstance(classify, nn.Dropout):
            layers.add_module('Dropout', classify)
        elif isinstance(classify, nn.ReLU):
            layers.add_module('ReLU', classify)
        else:
            raise exceptions.NotSupportedException(type(classify))

    return layers

def resnet18():
    r'''
    Return ResNet-18 layers which can be supported.
    '''
    conv_layer = ['conv_1',
                  'conv_1_0_1', 'conv_1_0_2',
                  'conv_1_1_1', 'conv_1_1_2',
                  'conv_2_0_1', 'conv_2_0_2',
                  'conv_2_1_1', 'conv_2_1_2',
                  'conv_3_0_1', 'conv_3_0_2',
                  'conv_3_1_1', 'conv_3_1_2',
                  'conv_4_0_1', 'conv_4_0_2',
                  'conv_4_1_1', 'conv_4_1_2']
    pool_layer = ['maxpool', 'avgpool']
    fc_layer = ['fc']

    tv_res18 = torchvision.models.resnet18(pretrained=False, progress=True)
    layers = nn.Sequential()

    conv_layer.reverse()
    pool_layer.reverse()

    layers.add_module(conv_layer.pop(), tv_res18.conv1)
    layers.add_module('BN', tv_res18.bn1)
    layers.add_module('ReLU', tv_res18.relu)
    layers.add_module(pool_layer.pop(), tv_res18.maxpool)

    res_layers = [tv_res18.layer1, tv_res18.layer2, tv_res18.layer3, tv_res18.layer4]

    for layer in res_layers:
        for basicblock in layer:
            layers.add_module(conv_layer.pop(), basicblock.conv1)
            layers.add_module('BN', basicblock.bn1)
            layers.add_module('ReLU', basicblock.relu)
            layers.add_module(conv_layer.pop(), basicblock.conv2)
            layers.add_module('BN', basicblock.bn2)

    layers.add_module(pool_layer.pop(), tv_res18.avgpool)
    layers.add_module(fc_layer.pop(), tv_res18.fc)

    return layers

def resnet50():
    r'''
    Return ResNet-50 layers which can be supported.
    '''
    conv_layer = ['conv_1',
                  'conv_1_0_1', 'conv_1_0_2', 'conv_1_0_3',
                  'conv_1_1_1', 'conv_1_1_2', 'conv_1_1_3',
                  'conv_1_2_1', 'conv_1_2_2', 'conv_1_2_3',
                  'conv_2_0_1', 'conv_2_0_2', 'conv_2_0_3',
                  'conv_2_1_1', 'conv_2_1_2', 'conv_2_1_3',
                  'conv_2_2_1', 'conv_2_2_2', 'conv_2_2_3',
                  'conv_2_3_1', 'conv_2_3_2', 'conv_2_3_3',
                  'conv_3_0_1', 'conv_3_0_2', 'conv_3_0_3',
                  'conv_3_1_1', 'conv_3_1_2', 'conv_3_1_3',
                  'conv_3_2_1', 'conv_3_2_2', 'conv_3_2_3',
                  'conv_3_3_1', 'conv_3_3_2', 'conv_3_3_3',
                  'conv_3_4_1', 'conv_3_4_2', 'conv_3_4_3',
                  'conv_3_5_1', 'conv_3_5_2', 'conv_3_5_3',
                  'conv_4_0_1', 'conv_4_0_2', 'conv_4_0_3',
                  'conv_4_1_1', 'conv_4_1_2', 'conv_4_1_3',
                  'conv_4_2_1', 'conv_4_2_2', 'conv_4_2_3']
    pool_layer = ['maxpool', 'avgpool']
    fc_layer = ['fc']

    tv_res50 = torchvision.models.resnet50(pretrained=False, progress=True)
    layers = nn.Sequential()

    conv_layer.reverse()
    pool_layer.reverse()

    layers.add_module(conv_layer.pop(), tv_res50.conv1)
    layers.add_module('BN', tv_res50.bn1)
    layers.add_module('ReLU', tv_res50.relu)
    layers.add_module(pool_layer.pop(), tv_res50.maxpool)

    res_layers = [tv_res50.layer1, tv_res50.layer2, tv_res50.layer3, tv_res50.layer4]

    for layer in res_layers:
        for bottleneck in layer:
            layers.add_module(conv_layer.pop(), bottleneck.conv1)
            layers.add_module('BN', bottleneck.bn1)
            layers.add_module(conv_layer.pop(), bottleneck.conv2)
            layers.add_module('BN', bottleneck.bn2)
            layers.add_module(conv_layer.pop(), bottleneck.conv3)
            layers.add_module('BN', bottleneck.bn3)
            layers.add_module('ReLU', bottleneck.relu)

    layers.add_module(pool_layer.pop(), tv_res50.avgpool)
    layers.add_module(fc_layer.pop(), tv_res50.fc)

    return layers

def squeezenet1_1():
    r'''
    Return SqueezeNet v1.1 layers which can be supported.
    '''
    conv_layer = ['conv_0',
                  'sqz_1', 'exp_1_1', 'exp_1_3',
                  'sqz_2', 'exp_2_1', 'exp_2_3',
                  'sqz_3', 'exp_3_1', 'exp_3_3',
                  'sqz_4', 'exp_4_1', 'exp_4_3',
                  'sqz_5', 'exp_5_1', 'exp_5_3',
                  'sqz_6', 'exp_6_1', 'exp_6_3',
                  'sqz_7', 'exp_7_1', 'exp_7_3',
                  'sqz_8', 'exp_8_1', 'exp_8_3',
                  'conv_c']
    pool_layer = ['pool1', 'pool2', 'pool3', 'pool4']

    tv_sqznet1_1 = torchvision.models.squeezenet1_1(pretrained=False, progress=True)
    tv_sqznet1_1.features[2].padding = 2
    tv_sqznet1_1.features[5].padding = 2
    tv_sqznet1_1.features[8].padding = 2
    layers = nn.Sequential()

    conv_layer.reverse()
    pool_layer.reverse()

    # Feature Extraction Layers
    for feature in tv_sqznet1_1.features:
        if isinstance(feature, nn.Conv2d):
            layers.add_module(conv_layer.pop(), feature)
        elif isinstance(feature, nn.ReLU):
            layers.add_module('ReLU', feature)
        elif isinstance(feature, nn.MaxPool2d):
            layers.add_module(pool_layer.pop(), feature)
        elif isinstance(feature, torchvision.models.squeezenet.Fire):
            layers.add_module(conv_layer.pop(), feature.squeeze)
            layers.add_module('ReLU', feature.squeeze_activation)
            layers.add_module(conv_layer.pop(), feature.expand1x1)
            layers.add_module('ReLU', feature.expand1x1_activation)
            layers.add_module(conv_layer.pop(), feature.expand3x3)
            layers.add_module('ReLU', feature.expand3x3_activation)
        else:
            raise exceptions.NotSupportedException(type(feature))

    # Classifier Layers
    for classify in tv_sqznet1_1.classifier:
        if isinstance(classify, nn.Dropout):
            layers.add_module('Dropout', classify)
        elif isinstance(classify, nn.Conv2d):
            layers.add_module(conv_layer.pop(), classify)
        elif isinstance(classify, nn.ReLU):
            layers.add_module('ReLU', classify)
        elif isinstance(classify, nn.AdaptiveAvgPool2d):
            layers.add_module(pool_layer.pop(), classify)
        else:
            raise exceptions.NotSupportedException(type(classify))

    return layers

def darknet19():
    r'''
    Return DarkNet19 layers which can supported.
    '''
    conv_layer = ['conv_0', 
                  'conv_1_1',
                  'conv_2_1', 'conv_2_2', 'conv_2_3',
                  'conv_3_1', 'conv_3_2', 'conv_3_3',
                  'conv_4_1', 'conv_4_2', 'conv_4_3', 'conv_4_4', 'conv_4_5',
                  'conv_5_1', 'conv_5_2', 'conv_5_3', 'conv_5_4', 'conv_5_5', 'conv_5_6']
    pool_layer = ['max_pool_1', 'max_pool_2', 'max_pool_3', 'max_pool_4', 'max_pool_5', 'avg_pool']

    layers = nn.Sequential()

    conv_layer.reverse()
    pool_layer.reverse()

    layers.add_module(conv_layer.pop(), nn.Conv2d(3, 32, 3, padding=1))
    layers.add_module(pool_layer.pop(), nn.MaxPool2d(2, stride=2))
    layers.add_module(conv_layer.pop(), nn.Conv2d(32, 64, 3, padding=1))
    layers.add_module(pool_layer.pop(), nn.MaxPool2d(2, stride=2))
    layers.add_module(conv_layer.pop(), nn.Conv2d(64, 128, 3, padding=1))
    layers.add_module(conv_layer.pop(), nn.Conv2d(128, 64, 1))
    layers.add_module(conv_layer.pop(), nn.Conv2d(64, 128, 3, padding=1))
    layers.add_module(pool_layer.pop(), nn.MaxPool2d(2, stride=2))
    layers.add_module(conv_layer.pop(), nn.Conv2d(128, 256, 3, padding=1))
    layers.add_module(conv_layer.pop(), nn.Conv2d(256, 128, 1))
    layers.add_module(conv_layer.pop(), nn.Conv2d(128, 256, 3, padding=1))
    layers.add_module(pool_layer.pop(), nn.MaxPool2d(2, stride=2))
    layers.add_module(conv_layer.pop(), nn.Conv2d(256, 512, 3, padding=1))
    layers.add_module(conv_layer.pop(), nn.Conv2d(512, 256, 1))
    layers.add_module(conv_layer.pop(), nn.Conv2d(256, 512, 3, padding=1))
    layers.add_module(conv_layer.pop(), nn.Conv2d(512, 256, 1))
    layers.add_module(conv_layer.pop(), nn.Conv2d(256, 512, 3, padding=1))
    layers.add_module(pool_layer.pop(), nn.MaxPool2d(2, stride=2))
    layers.add_module(conv_layer.pop(), nn.Conv2d(512, 1024, 3, padding=1))
    layers.add_module(conv_layer.pop(), nn.Conv2d(1024, 512, 1))
    layers.add_module(conv_layer.pop(), nn.Conv2d(512, 1024, 3, padding=1))
    layers.add_module(conv_layer.pop(), nn.Conv2d(1024, 512, 1))
    layers.add_module(conv_layer.pop(), nn.Conv2d(512, 1024, 3, padding=1))
    layers.add_module(conv_layer.pop(), nn.Conv2d(1024, 1000, 1))
    layers.add_module(pool_layer.pop(), nn.AdaptiveAvgPool2d(1000))

    return layers

def yolov2():
    r'''
    Return YOLOv2 layers which can be supported.
    '''
    conv_layer = ['conv_1',
                  'conv_2',
                  'conv_3_1', 'conv_3_2', 'conv_3_3',
                  'conv_4_1', 'conv_4_2', 'conv_4_3',
                  'conv_5_1', 'conv_5_2', 'conv_5_3', 'conv_5_4', 'conv_5_5',
                  'conv_6_1', 'conv_6_2', 'conv_6_3', 'conv_6_4', 'conv_6_5', 'conv_6_6', 'conv_6_7',
                  'conv_7',
                  'conv_8',
                  'conv_9']
    pool_layer = ['max_pool_1', 'max_pool_2', 'max_pool_3', 'max_pool_4', 'max_pool_5']

    layers = nn.Sequential()

    conv_layer.reverse()
    pool_layer.reverse()

    layers.add_module(conv_layer.pop(), nn.Conv2d(3, 32, 3, padding=1))
    layers.add_module(pool_layer.pop(), nn.MaxPool2d(2, stride=2))
    layers.add_module(conv_layer.pop(), nn.Conv2d(32, 64, 3, padding=1))
    layers.add_module(pool_layer.pop(), nn.MaxPool2d(2, stride=2))
    layers.add_module(conv_layer.pop(), nn.Conv2d(64, 128, 3, padding=1))
    layers.add_module(conv_layer.pop(), nn.Conv2d(128, 64, 1))
    layers.add_module(conv_layer.pop(), nn.Conv2d(64, 128, 3, padding=1))
    layers.add_module(pool_layer.pop(), nn.MaxPool2d(2, stride=2))
    layers.add_module(conv_layer.pop(), nn.Conv2d(128, 256, 3, padding=1))
    layers.add_module(conv_layer.pop(), nn.Conv2d(256, 128, 1))
    layers.add_module(conv_layer.pop(), nn.Conv2d(128, 256, 3, padding=1))
    layers.add_module(pool_layer.pop(), nn.MaxPool2d(2, stride=2))
    layers.add_module(conv_layer.pop(), nn.Conv2d(256, 512, 3, padding=1))
    layers.add_module(conv_layer.pop(), nn.Conv2d(512, 256, 1))
    layers.add_module(conv_layer.pop(), nn.Conv2d(256, 512, 3, padding=1))
    layers.add_module(conv_layer.pop(), nn.Conv2d(512, 256, 1))
    layers.add_module(conv_layer.pop(), nn.Conv2d(256, 512, 3, padding=1))
    layers.add_module(pool_layer.pop(), nn.MaxPool2d(2, stride=2))
    layers.add_module(conv_layer.pop(), nn.Conv2d(512, 1024, 3, padding=1))
    layers.add_module(conv_layer.pop(), nn.Conv2d(1024, 512, 1))
    layers.add_module(conv_layer.pop(), nn.Conv2d(512, 1024, 3, padding=1))
    layers.add_module(conv_layer.pop(), nn.Conv2d(1024, 512, 1))
    layers.add_module(conv_layer.pop(), nn.Conv2d(512, 1024, 3, padding=1))
    layers.add_module(conv_layer.pop(), nn.Conv2d(1024, 1024, 3, padding=1))
    layers.add_module(conv_layer.pop(), nn.Conv2d(1024, 1024, 3, padding=1))
    layers.add_module(conv_layer.pop(), nn.Conv2d(512, 64, 1))
    layers.add_module(conv_layer.pop(), nn.Conv2d(1280, 1024, 3, padding=1))
    layers.add_module(conv_layer.pop(), nn.Conv2d(1024, 425, 1))

    return layers