# This file is Copyright (c) 2020 Felix Arnold <felix.arnold@gmail.com>
# License: BSD

from migen import *
from migen.genlib.misc import WaitTimer
from litex.soc.interconnect.csr import *

# TDC ------------------------------------------------------------------------

# modes definition
_STOPPED_MODE  = 0
_COUNTING_MODE = 1

class Tdc(Module, AutoCSR):
    def __init__(self, sys_clk_freq, period=1e0):

        # number of bits
        n = 30

        # Configuration and Status Registers (CSR)
        self._test = CSRStatus(n, description="Test")
        self._counter = CSRStatus(n, description="Counter")
        self._control = CSRStorage(n, description="Control")

        # signals
        mode = Signal(reset = _STOPPED_MODE)
        counter = Signal(n)

        # handle start and stop signals
        start = Signal()
        self.comb += start.eq(self._control.storage[0])
        stop = Signal()
        self.comb += stop.eq(self._control.storage[1])

        #  counter and mode update
        self.sync += If(mode == _COUNTING_MODE, counter.eq(counter + 1))
        self.sync += If(start, mode.eq(_COUNTING_MODE))
        self.sync += If(start, counter.eq(0))
        self.sync += If(stop, mode.eq(_STOPPED_MODE))

        # set registerts
        self.comb += self._test.status.eq(0x00010002) # test register
        self.comb += self._counter.status.eq(counter)

