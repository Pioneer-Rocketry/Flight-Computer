"""python -m navsitl synthetic|replay|list-scenarios"""

import argparse
import json
from dataclasses import asdict
from pathlib import Path

from .abi import run_rows
from .report import write_report
from .schema import read_rows, write_rows
from .trajectory import SCENARIOS, SensorConfig, generate


def main(argv=None):
    parser = argparse.ArgumentParser(description="Run the production C++ navigation estimator on a host")
    commands = parser.add_subparsers(dest="command", required=True)
    commands.add_parser("list-scenarios")
    synthetic = commands.add_parser("synthetic")
    synthetic.add_argument("scenario", choices=SCENARIOS)
    synthetic.add_argument("--duration-s", type=float)
    synthetic.add_argument("--dt-us", type=int, default=10000)
    synthetic.add_argument("--seed", type=int, default=1)
    synthetic.add_argument("--sensor-config", type=Path,
                           help="JSON keys from SensorConfig; parameters are illustrative")
    synthetic.add_argument("--save-input", action="store_true")
    synthetic.add_argument("--output", type=Path, required=True)
    synthetic.add_argument("--plots", action="store_true")
    replay = commands.add_parser("replay")
    replay.add_argument("input", type=Path)
    replay.add_argument("--metadata", type=Path)
    replay.add_argument("--output", type=Path, required=True)
    replay.add_argument("--plots", action="store_true")
    args = parser.parse_args(argv)
    if args.command == "list-scenarios":
        print("\n".join(SCENARIOS))
        return 0
    if args.command == "synthetic":
        config = (SensorConfig(**json.loads(args.sensor_config.read_text()))
                  if args.sensor_config else SensorConfig())
        rows = generate(args.scenario, args.duration_s, args.dt_us, args.seed, config)
        if args.save_input:
            write_rows(args.output / "input.csv", rows,
                       {"scenario": args.scenario, "seed": args.seed,
                        "sensor_parameters": asdict(config)})
    else:
        rows, _ = read_rows(args.input, args.metadata)
    summary = write_report(args.output, run_rows(rows), plots=args.plots)
    print(json.dumps(summary, indent=2))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
