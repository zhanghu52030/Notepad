#include "notepad/text_action.h"

namespace csi_training {

///////////////////////////////////////////////////////////////////////////////

TextAction::TextAction(TextBuffer* text_buffer,
                       ActionDir action_dir,
                       wxChar action_char,
                       const wxPoint& action_position,
                       const wxPoint& after_action_position)
    : text_buffer_{ text_buffer }
    , action_dir_{ action_dir }
    , action_char_{ action_char }
    , action_position_{ action_position }
    , after_action_position_{ after_action_position } {
}

DeleteAction::DeleteAction(TextBuffer* text_buffer,
                           ActionDir action_dir,
                           wxChar action_char,
                           const wxPoint& action_position,
                           const wxPoint& after_action_position)
    : TextAction(text_buffer, action_dir,
                 action_char, action_position,
                 after_action_position) {
}

void DeleteAction::Undo() {
  text_buffer()->InsertChar(after_action_position(),
                            action_char(),
                            action_dir(),
                            true);
}

void DeleteAction::Redo() {
  text_buffer()->DeleteChar(action_position(), action_dir(), true);
}

////////////////////////////////////////////////////////////////////////////////

InsertAction::InsertAction(TextBuffer* text_buffer,
                           ActionDir action_dir,
                           wxChar action_char,
                           const wxPoint& action_position,
                           const wxPoint& after_action_position)
    : TextAction(text_buffer, action_dir,
                 action_char, action_position,
                 after_action_position) {
}

void InsertAction::Undo() {
  text_buffer()->DeleteChar(after_action_position(),
                            action_dir(),
                            true);
}

void InsertAction::Redo() {
  text_buffer()->InsertChar(action_position(),
                            action_char(),
                            action_dir(),
                            true);
}

}  // namespace csi_training
