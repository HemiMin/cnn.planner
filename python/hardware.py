r"""
Description of hardware spec
- frequency
- bandwidth
- multiply-accumulate energy
- on-chip access energy
- off-chip access energy
"""

import json

class HardwareSpec:
    r"""
    Description of hardware spec
    """
    # pylint: disable=too-many-instance-attributes
    # Hardware needs many attributes

    def __init__(self, cfg):
        self._cfg = None
        with open(cfg) as cfg_f:
            self._cfg = json.load(cfg_f)
        self._mac_cycles = self._cfg['mac_cycles']
        self._bandwidth = self._cfg['bandwidth']
        self._frequency = self._cfg['frequency']
        self._mac_energy = self._cfg['mac_energy']
        self._on_chip_energy_32 = self._cfg['on_chip_energy_32']
        self._off_chip_energy_32 = self._cfg['off_chip_energy_32']
        self._mem_size = self._cfg['mem_size']
        self._pe_dim = self._cfg['pe_dim']
        self._pe_strt = self._cfg['pe_structure']

        for i, row in enumerate(self._pe_strt):
            for j, element in enumerate(row):
                if element == 'KW':
                    self._pe_strt[i][j] = 1
                elif element == 'KH':
                    self._pe_strt[i][j] = 2
                elif element == 'IC':
                    self._pe_strt[i][j] = 3
                elif element == 'OW':
                    self._pe_strt[i][j] = 4
                elif element == 'OH':
                    self._pe_strt[i][j] = 5
                elif element == 'OC':
                    self._pe_strt[i][j] = 6
                elif element == 'IW':
                    self._pe_strt[i][j] = 7
                elif element == 'IH':
                    self._pe_strt[i][j] = 8
                else:
                    self._pe_strt[i][j] = 0

    @property
    def mac_cycles(self):
        r"""Get MAC cycles for calculating one period"""
        return self._mac_cycles

    @property
    def frequency(self):
        r"""Get PE frequency"""
        return self._frequency

    @property
    def bandwidth(self):
        r"""Get off-chip bandwidth"""
        return self._bandwidth

    @property
    def mac_energy(self):
        r"""Get multiply-accumulator unit energy"""
        return self._mac_energy

    @property
    def on_chip_energy_32(self):
        r"""Get on-chip access energy for 32 bits data"""
        return self._on_chip_energy_32

    @property
    def off_chip_energy_32(self):
        r"""Get off-chip access energy for 32 bits data"""
        return self._off_chip_energy_32

    @property
    def mem_size(self):
        r"""Get memory size"""
        return self._mem_size

    @property
    def pe_dim(self):
        r"""Get PE's physical dimension"""
        return self._pe_dim

    @property
    def pe_strt(self):
        r"""
        Get PE width and height dimension distribution.
        For example, PE width stands for IC*KH*KW
        and PE height stands for OC*OH*OW
        """
        return self._pe_strt

    @mac_cycles.setter
    def mac_cycels(self, mac_cycles):
        self._mac_cycles = mac_cycles

    @frequency.setter
    def frequency(self, frequency):
        self._frequency = float(frequency)

    @bandwidth.setter
    def bandwidth(self, bandwidth):
        self._bandwidth = float(bandwidth)

    @mac_energy.setter
    def mac_energy(self, mac_energy):
        self._mac_energy = float(mac_energy)

    @on_chip_energy_32.setter
    def on_chip_energy_32(self, on_chip_energy_32):
        self._on_chip_energy_32 = float(on_chip_energy_32)

    @off_chip_energy_32.setter
    def off_chip_energy_32(self, off_chip_energy_32):
        self._off_chip_energy_32 = float(off_chip_energy_32)

    @mem_size.setter
    def mem_size(self, mem_size):
        self._mem_size = mem_size

        for i, size in enumerate(self._mem_size):
            self._mem_size[i] = size * 1024  # Trans KB to Byte

    @pe_dim.setter
    def pe_dim(self, pe_dim):
        self._pe_dim = pe_dim

    @pe_strt.setter
    def pe_strt(self, pe_strt):
        self._pe_strt = pe_strt
        for i, row in enumerate(self._pe_strt):
            for j, element in enumerate(row):
                if element == 'KW':
                    self._pe_strt[i][j] = 1
                elif element == 'KH':
                    self._pe_strt[i][j] = 2
                elif element == 'IC':
                    self._pe_strt[i][j] = 3
                elif element == 'OW':
                    self._pe_strt[i][j] = 4
                elif element == 'OH':
                    self._pe_strt[i][j] = 5
                elif element == 'OC':
                    self._pe_strt[i][j] = 6
                elif element == 'IW':
                    self._pe_strt[i][j] = 7
                elif element == 'IH':
                    self._pe_strt[i][j] = 8
                else:
                    self._pe_strt[i][j] = 0
