#!/bin/bash
SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

cd "${SCRIPT_DIR}"
cd ../../../assets/sound;
TOP=$(pwd)
COUNT=0
> lengths
function LINE(){
  echo "$*" >> lengths
}
function NO_LINE(){
  echo -n "$*" >> lengths
}
LINE '#ifndef __WAVE_LEN_HEADER__'
LINE '#define __WAVE_LEN_HEADER__'
LINE '#include <iostream>'
LINE '#include <array>'
LINE 'namespace sound::wave_length {'
LINE ' static const std::array<std::pair<std::string,float>,___COUNT___> list = {'
for snd in $(find . -type d); do
  cd "$TOP"
  cd "$snd"
  for file in $(find . -type f -path '*.wav'); do
    NO_LINE 'std::make_pair<>("../assets/sound/'
    NO_LINE $(echo -n "${file}" | sed -E 's|^./||')
    NO_LINE '",'
    NO_LINE $(soxi -D "$file" 2>/dev/null)
    LINE '),'
    COUNT=$(( COUNT + 1 ))
  done
done
cd "$TOP"
sed -E "s|,float>,___COUNT___>|,float>,${COUNT}>|" -i lengths
LINE '};'
LINE '};'
LINE '#endif'

mv lengths "${SCRIPT_DIR}/../sound/wave-length.hpp"
cd "${SCRIPT_DIR}"
