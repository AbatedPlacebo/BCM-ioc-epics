#!/bin/bash

for pv in a b c; do
cat<<END
record(calc, "\$(P)heart_${pv}") {
  field(CALC, "A+1")
  field(INPA, "\$(P)heart_${pv}")
  field(SCAN, "1 second")
}
END
done
