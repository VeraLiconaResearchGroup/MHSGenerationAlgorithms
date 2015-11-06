#!/usr/bin/env python

import sys
import random

def gen_spectra(comps, trans, a, e):
    ret = ["%d %d" % (comps, trans)]
    t = 0
    while (t < trans):
        err = random.random() < e
        ok = False
        tmp_str = ""

        for c in range(comps):
            if(random.random() < a):
                tmp_str += "1 "
                ok = True;
            else:
                tmp_str += "0 "
        if not ok:
            continue
        if err:
            tmp_str += "x"
        else:
            tmp_str += "."
        t += 1
        ret.append(tmp_str)

    return "\n".join(ret)


def main(argv):
    if (not (3 <= len(argv) <= 5)):
        sys.exit('Usage: %s <#components> <#transactions> [activation_rate] [error_rate]' % argv[0])

    component_count = int(argv[1])
    transaction_count = int(argv[2])

    if (len(argv) > 3):
        activation_rate = float(argv[3])
    else:
        activation_rate = random.uniform(0.1, 0.9)

    if (len(argv) > 4):
        error_rate = float(argv[4])
    else:
        error_rate = random.uniform(0.1, 0.9)

    print(gen_spectra(component_count,
                      transaction_count,
                      activation_rate,
                      error_rate))

if __name__ == "__main__":
    main(sys.argv)
