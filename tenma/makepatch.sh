versionnum="v1.2"

# filename_iso="patch/auto_patch/Tengai Makyou - Ziria EN [${versionnum}-iso].xdelta"
# filenameredump="patch/redump_patch/Tengai Makyou Ziria EN [${versionnum}] Redump.xdelta"
# filenamesplitbin="patch/splitbin_patch/Tengai Makyou Ziria EN [${versionnum}] SplitBin.xdelta"
filename_rev6="patch/auto_patch/Tengai Makyou - Ziria EN [${versionnum}-iso-rev6].xdelta"
filename_rev7="patch/auto_patch/Tengai Makyou - Ziria EN [${versionnum}-iso-rev7].xdelta"
filenameredump_rev6="patch/redump_patch/Tengai Makyou - Ziria EN [${versionnum}-rev6] Redump.xdelta"
filenameredump_rev7="patch/redump_patch/Tengai Makyou - Ziria EN [${versionnum}-rev7] Redump.xdelta"
filenamesplitbin_rev6="patch/splitbin_patch/Tengai Makyou - Ziria EN [${versionnum}-rev6] SplitBin.xdelta"
filenamesplitbin_rev7="patch/splitbin_patch/Tengai Makyou - Ziria EN [${versionnum}-rev7] SplitBin.xdelta"

mkdir -p patch
mkdir -p patch/auto_patch
mkdir -p patch/redump_patch
mkdir -p patch/splitbin_patch


./build.sh

rm -f "$filename"
#xdelta3 -e -f -B 202635264 -s "patch/exclude/tenma_rev6_02.iso" "tenma_02_build.iso" "$filename_rev6"
#xdelta3 -e -f -B 202635264 -s "patch/exclude/tenma_rev7_02.iso" "tenma_02_build.iso" "$filename_rev7"
xdelta3 -e -f -B 128966656 -s "patch/exclude/tenma_rev6_02.iso" "tenma_02_build.iso" "$filename_rev6"
xdelta3 -e -f -B 128966656 -s "patch/exclude/tenma_rev7_02.iso" "tenma_02_build.iso" "$filename_rev7"

rm -f "$filenameredump"
../discaster/discaster tenma.dsc
xdelta3 -e -f -B 370047216 -s "patch/exclude/tenma_rev6.bin" "tenma_build.bin" "$filenameredump_rev6"
xdelta3 -e -f -B 370047216 -s "patch/exclude/tenma_rev7.bin" "tenma_build.bin" "$filenameredump_rev7"
