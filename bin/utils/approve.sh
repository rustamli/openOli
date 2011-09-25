# sends submit to results and removes executable binary file

SBM="$1"
SBMID="$2"

cd $SBM
rm -f program
cd ..
mv $SBMID "../results"
