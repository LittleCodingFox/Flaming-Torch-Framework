for f in $2/*;
do
  export filename=`basename $f`
  echo Packing $filename to "$3/$filename.package"
  $1 -dir $f "" -out "$3/$filename.package";
done
