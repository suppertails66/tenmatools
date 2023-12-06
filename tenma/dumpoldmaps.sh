#set -o errexit

make tenma_maprender_generic

mkdir -p scrap/oldmaps
mkdir -p scrap/oldmaps2

# for i in `seq 0 20`; do
#   ./tenma_maprender_generic tenma_02.iso $((0xCB76-(8*56)+(8*$i))) scrap/oldmaps/$i.png --old
# #  echo $i
# done

# for i in `seq 0 113`; do
#   echo $i
#   ./tenma_maprender_generic tenma_02.iso $((0xC9B6-(8*0)+(8*$i))) scrap/oldmaps/$i.png --old
# done

for i in `seq 0 128`; do
  echo $i
  ./tenma_maprender_generic tenma_02.iso $((0xCD38+(8*$i))) scrap/oldmaps2/$i.png --old
done
