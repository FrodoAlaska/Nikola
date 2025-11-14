#include "nikola/nikola_ui.h"
#include "nikola/nikola_math.h"

#include <RmlUi/Core.h>

//////////////////////////////////////////////////////////////////////////

namespace nikola { // Start of nikola

///---------------------------------------------------------------------------------------------------------------------
/// UIDocument functions

UIDocument* ui_document_load(UIContext* ui_ctx, const FilePath& doc_path) {
  NIKOLA_ASSERT(ui_ctx, "Invalid UIContext given to ui_document_load");
  return ui_ctx->LoadDocument(doc_path);
}

UIDocument* ui_document_load_from_memory(UIContext* ui_ctx, const String& doc_src) {
  NIKOLA_ASSERT(ui_ctx, "Invalid UIContext given to ui_document_load_from_memory");
  return ui_ctx->LoadDocumentFromMemory(doc_src);
}

UIDocument* ui_document_create(UIContext* ui_ctx, const String& maker_name) {
  NIKOLA_ASSERT(ui_ctx, "Invalid UIContext given to ui_document_create");
  return ui_ctx->CreateDocument(maker_name);
}

void ui_document_show(UIDocument* ui_doc) {
  NIKOLA_ASSERT(ui_doc, "Invalid UIDocument given to ui_document_show");
  ui_doc->Show();
}

} // End of nikola

//////////////////////////////////////////////////////////////////////////
