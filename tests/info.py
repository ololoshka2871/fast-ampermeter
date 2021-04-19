 # based on https://github.com/walac/pyusb/blob/master/docs/tutorial.rst

import usb.core
import usb.util

# find our device
#dev = usb.core.find(idVendor=0x0483, idProduct=0x5750)
dev = usb.core.find(idVendor=0x046d, idProduct=0xc05b)


# was it found?
if dev is None:
    raise ValueError('Device not found')

dev.set_configuration()

cfg=dev[0]
intf=cfg[(0,0)]
ep=intf[0]

# dev.write(ep.bEndpointAddress, [0x00, 0x00,0x04,0x04,0xFF,0xFF,0xFF,0x00, 0x00], 1000)
# dev.ctrl_transfer(bmRequestType, bRequest, wValue=0, wIndex=0, data_or_wLength=None, timeout=None)

print("print ep")
print(ep)
print("print cfg")
print(cfg)
print("print intf")
print(intf)
