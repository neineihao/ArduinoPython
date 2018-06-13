import visa
import time

class GPIB_Manager():
    def __init__(self):
        self.rm = visa.ResourceManager()
        self.inst = None

    def __enter__(self):
        eq = self.rm.list_resources()
        if len(eq) > 0:
            print("Equipment find !")
            resource = eq[0]
            self.inst = self.rm.open_resource(resource)
            print(self.inst.query("*IDN?"))
            return self
        else:
            raise IOError("No Equipment Find")

    def write(self, command):
        self.inst.write(command)

    def write_data(self, data, value, channel):
        data_map = {}
        data_map['v'] = "VOLT"
        data_map['i'] = "CURR"
        write_string = "{} {}, (@{})".format(data_map[data], value, channel)
        self.write(write_string)

    def turn_on(self, channel):
        write_string = "OUTP ON, (@{})".format(channel)
        self.write(write_string)

    def turn_off(self, channel):
        write_string = 'OUTP OFF, (@{})'.format(channel)
        self.write(write_string)

    def __exit__(self, exc_type, exc_val, exc_tb):
        self.inst.close()
        self.rm.close()

if __name__ == '__main__':
    # GPIB = GPIB_Manager()
    # GPIB.write("CURR 0.6, (@1)")
    # GPIB.write("VOLT 2.000, (@1)")
    #
    # GPIB.write('OUTP ON, (@1)')
    # GPIB.end()
    with GPIB_Manager() as g_m:
        g_m.write_data('v', 2.000, 2)
        g_m.write_data('i', 0.6, 2)
        g_m.turn_on(2)
        time.sleep(5)
        g_m.turn_off(2)
