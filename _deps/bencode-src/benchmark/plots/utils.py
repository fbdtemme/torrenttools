import pandas as pd
import numpy as np
from pathlib import Path
import json
import matplotlib.pyplot as plt


def load_json_benchmarks(file_path: Path) -> pd.DataFrame:
    """Load the benchmark results from a google benchmark json file."""
    with open(file_path, "r") as f:
        data = json.load(f)
    
    return pd.DataFrame(data["benchmarks"])


def filter_results(df: pd.DataFrame, library: str) -> pd.DataFrame:
    """Remove mean, median and stddev from the results."""
    df_means = df.name.str.contains("mean")
    df_medians = df.name.str.contains("median") 
    df_stddev = df.name.str.contains("stddev") 

    filtered_df = df.loc[~(df_means|df_medians|df_stddev),:]
    filtered_df = filtered_df.reset_index(drop=True)
    test_data = filtered_df.name.str.split('/').apply(
        lambda l: (l[0].replace("BM_", ""), l[1].split("_")[0][1:-1])
    )
    filtered_df = filtered_df.drop(columns="name")
    test_data = pd.DataFrame(test_data.tolist(), columns=("test_type", "test_file"))

    out = pd.merge(test_data, filtered_df, left_index=True, right_index=True)

    out.insert(0, "library", library)


    out["cpu_time/iteration"] = np.nan_to_num(out.cpu_time / out.iterations, nan=0)
    out["real_time/iteration"] = np.nan_to_num(out.real_time / out.iterations, nan=0)
    
    out["library"] = out['library'].astype("category")
    out["test_type"] = out['test_type'].astype("category")
    out["test_file"] = out['test_file'].astype("category")

    return out


def plot_comparison(df, **figure_options):
    libraries = df.index.get_level_values("library").unique()
    n_libraries = len(np.unique(libraries))

    test_files = df.index.get_level_values("test_file").unique()
    n_test_files = len(np.unique(test_files))

    library_values = df.index.get_level_values("library")
    means = np.ndarray((n_libraries, n_test_files))
    stddevs = np.ndarray((n_libraries, n_test_files))

    for idx, lib in enumerate(libraries):
        means[idx, :] = df.loc[library_values == lib, "mean"]
    
    for idx, lib in enumerate(libraries):
        stddevs[idx, :] = df.loc[library_values == lib, "stddev"]


    # scale_factor = 2**30
    scale_factor = np.max(means, axis=0)
    # normalize per column
    means = means / scale_factor
    stddevs = stddevs / scale_factor

    fig = plt.figure(**figure_options)
    ax = fig.gca()

    x = np.arange(n_test_files)
    extra_space = 3
    per_group_width = n_libraries+extra_space
    width = 1 / per_group_width  # the width of the bars

    rects = []
    for i in range(n_libraries):
        mean = means[i, :]
        std = stddevs[i, :]
        rects.append(
            ax.bar(x + i*width, mean, width=width, yerr=std, label=test_files)
        )

    ax.set_ylabel("Relative parsing speed")

    # Set the position of the x ticks
    ax.set_xticks(x + (n_libraries-1) * width * 0.5)
    # Set the labels for the x ticks
    ax.set_xticklabels(test_files)

    ax.legend(libraries, bbox_to_anchor=(1.05, 1), loc='upper left')

    return ax
