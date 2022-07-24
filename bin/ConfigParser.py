#-*- coding: utf-8 -*-

class ConfigParser():
  def __init__(self, file: str):
    self.file = file
    self.config = {}

  def __getitem__(self, key):
    self.read()
    if key in self.config.keys():
      return self.config[key]
    else:
      return ""

  def __setitem__(self, key, val):
    self.config[key] = val
    self.write()

  def read(self):
    with open(self.file, 'r') as conf:
      self.config = {}
      for line in conf.readlines():
        line = line.replace(' ', '').rstrip()
        key, value = line.split('=')
        self.config[key] = value

  def write(self):
    with open(self.file, 'w') as conf:
      for k, v in self.config.items():
        conf.write(k+'='+v+'\n')

  def keys(self):
    self.read()
    return self.config.keys()
