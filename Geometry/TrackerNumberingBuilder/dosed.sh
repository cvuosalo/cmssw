#!/bin/csh

foreach tagname ( container trans phi rho rot shape ddd ddname type params geographicalID radLength xi pixROCRows \
pixROCCols pixROCx pixROCy stereo siliconAPVNum )
  foreach target ( $* )
    echo Changing $tagname in $target
    sed "s/\([^a-z]\)_${tagname}/\1${tagname}_/g" $target > file.tmp
    /bin/mv file.tmp $target
  end
end
