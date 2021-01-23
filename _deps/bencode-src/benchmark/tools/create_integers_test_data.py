#!/usr/bin/env python3

import numpy as np
import argparse


def main(n_values):
    min_value = np.iinfo(np.int64).min
    max_value = np.iinfo(np.int64).max

    values = np.random.randint(min_value, max_value, n_values)
    print('l', end='')
    print("".join([f"i{v}e" for v in values]), end='')
    print('e', end='')

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("n", type=int)
    args = parser.parse_args()
    main(n_values=args.n)

