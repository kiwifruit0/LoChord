#!/bin/sh
# LoChord manufacturing outputs (JLCPCB-flavoured).
# Regenerates lochord_hw/production/ from the current schematic and board.
# Outputs are gitignored - run this script rather than committing them.
set -e
HERE=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
B="$HERE/lochord_hw.kicad_pcb"
S="$HERE/lochord_hw.kicad_sch"
OUT="$HERE/production"
rm -rf "$OUT"; mkdir -p "$OUT/gerbers"

kicad-cli pcb export gerbers --output "$OUT/gerbers/" \
  --layers "F.Cu,In1.Cu,In2.Cu,B.Cu,F.Paste,B.Paste,F.SilkS,B.SilkS,F.Mask,B.Mask,Edge.Cuts" \
  --no-protel-ext --subtract-soldermask --use-drill-file-origin "$B"

kicad-cli pcb export drill --output "$OUT/gerbers/" --format excellon \
  --drill-origin plot --excellon-separate-th --generate-map --map-format gerberx2 "$B"

kicad-cli pcb export pos --output "$OUT/lochord_hw-pos.csv" --format csv \
  --units mm --side both --use-drill-file-origin --exclude-dnp "$B"

kicad-cli sch export bom --output "$OUT/lochord_hw-bom.csv" \
  --fields 'Reference,Value,Footprint,LCSC,${QUANTITY}' \
  --labels 'Designator,Comment,Footprint,LCSC,Qty' \
  --group-by 'Value,Footprint,LCSC' --exclude-dnp "$S"

kicad-cli sch export pdf --output "$OUT/lochord_hw-schematic.pdf" "$S"
kicad-cli pcb export pdf  --output "$OUT/lochord_hw-fab.pdf" \
  --layers "F.Cu,In1.Cu,In2.Cu,B.Cu,F.SilkS,B.SilkS,Edge.Cuts" --mode-multipage "$B"

( cd "$OUT/gerbers" && zip -q ../lochord_hw-gerbers.zip ./* )
echo "--- production outputs ---"
ls -la "$OUT" "$OUT/gerbers" | head -40
