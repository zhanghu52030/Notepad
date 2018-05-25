#ifndef NOTEPAD_NOTEPAD_TEXT_ACTION_H_
#define NOTEPAD_NOTEPAD_TEXT_ACTION_H_

#include "wx/defs.h"
#include "wx/gdicmn.h"
#include "wx/string.h"

#include "notepad/defs.h"
#include "notepad/text_buffer.h"

namespace csi_training {

class TextBuffer;

class TextAction {
  wxDECLARE_NO_COPY_CLASS(TextAction);

 public:
  virtual ~TextAction() {}

  virtual void Undo() = 0;
  virtual void Redo() = 0;

  TextBuffer* text_buffer() const { return text_buffer_; }
  ActionDir action_dir() const { return action_dir_; }
  wxChar action_char() const { return action_char_; }
  const wxPoint& action_position() const { return action_position_; }
  const wxPoint& after_action_position() const {
    return after_action_position_;
  }

 protected:
  TextAction(TextBuffer* text_buffer,
             ActionDir action_dir,
             wxChar action_char,
             const wxPoint& action_position,
             const wxPoint& after_action_position);

 private:
  TextBuffer* text_buffer_;
  ActionDir action_dir_;
  wxChar action_char_;
  wxPoint action_position_;
  wxPoint after_action_position_;
};

class DeleteAction : public TextAction {
 public:
  DeleteAction(TextBuffer* text_buffer,
               ActionDir action_dir,
               wxChar action_char,
               const wxPoint& action_position,
               const wxPoint& after_action_position);

  ~DeleteAction() {}

  void Undo() override;
  void Redo() override;
};

class InsertAction : public TextAction {
 public:
  InsertAction(TextBuffer* text_buffer,
               ActionDir action_dir,
               wxChar action_char,
               const wxPoint& action_position,
               const wxPoint& after_action_position);

  ~InsertAction() override {}

  void Undo() override;
  void Redo() override;
};

}  // namespace csi_training

#endif  // NOTEPAD_NOTEPAD_TEXT_ACTION_H_
