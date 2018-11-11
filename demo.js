const { bgWhite } = require('chalk');
const readline = require('readline');
const { VolumeControl } = require("./");


console.log('');
console.log('up/right   - volume up');
console.log('down/left  - volume down');
console.log('m          - mute/unmute');
console.log('esc        - quit');
console.log('');

const volumeControl = new VolumeControl();


readline.emitKeypressEvents(process.stdin);
process.stdin.setRawMode(true);

process.stdin.on('keypress', (str, key) => {
  switch (key.name) {
    case 'escape':
      process.exit();
    case 'right':
    case 'up':
      volumeControl.setVolume(Math.min(1.0, volumeControl.getVolume() + 0.01));
      break;
    case 'down':
    case 'left':
      volumeControl.setVolume(Math.max(0.0, volumeControl.getVolume() - 0.01));
      break;
    case 'm': {
      volumeControl.setMuted(!volumeControl.isMuted());
      break;
    }
  }
  drawBar(volumeControl.getVolume());
});


const getBar = (length, char) => {
  let str = '';
  for (let i = 0; i < length; i += 1) {
    str += char;
  }
  return str;
};

const drawBar = (current) => {
  const barLength = 50;
  const percentageProgress = (current * 100).toFixed(0);
  this.currentProgress = percentageProgress;

  const filledBarLength = volumeControl.isMuted() ? 0 : (current * barLength).toFixed(0);


  const emptyBarLength = barLength - filledBarLength;

  const filledBar = bgWhite(getBar(filledBarLength, ' '));
  const emptyBar = getBar(emptyBarLength, '·');
  const title = 'Volume: ';
  let emoticon = '\u{1F50A}';
  if (percentageProgress < 40) {
    emoticon = '\u{1F509}';
  }
  if (percentageProgress < 10) {
    emoticon = '\u{1F508}';
  }
  if (volumeControl.isMuted()) {
    emoticon = '\u{1F507}';
  }

  process.stdout.clearLine();
  process.stdout.cursorTo(0);
  process.stdout.write(`${title}  ${emoticon}  [${filledBar}${emptyBar}] | ${percentageProgress}%            `);
}

drawBar(volumeControl.getVolume());