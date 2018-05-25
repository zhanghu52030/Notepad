#ifndef NOTEPAD_NOTEPAD_DEF_H_
#define NOTEPAD_NOTEPAD_DEF_H_

namespace csi_training {
// Commonly used constant chars.
const wxChar kTabChar = L'\t';
const wxChar kSpaceChar = L' ';
const int kInvInt = -1;
const char kShiftCharArray[] = "~!@#$%^&*()_+<>?:\"{}|";
const char kNonShiftCharArray[] = "`1234567890-=,./;'[]\\";
const wxChar CR = L'\r';
const wxChar LF = L'\n';
const wxChar BS = L'\b';

enum ActionDir {
  kForward = 0,
  kBackward,
  kUpward,
  kDownward
};

enum ActionType {
  kDelete = 0,
  kInsert
};

}  // namespace csi_training
#endif  // NOTEPAD_NOTEPAD_APP_H_
