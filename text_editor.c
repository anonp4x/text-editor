#include <gtk/gtk.h>
#include <glib.h>

GtkTextBuffer *buffer;  // Text buffer for the text view
GtkTextView *textView;  // Text view widget
GtkLabel *lineCounter;   // Label to display the line count

// Function to update the line counter
void updateLineCounter() {
    GtkTextIter startIter, endIter;
    gint lineCount = 0;

    gtk_text_buffer_get_bounds(buffer, &startIter, &endIter);

    // Count lines in the buffer
    while (gtk_text_iter_forward_line(&startIter)) {
        lineCount++;
    }
    lineCount++;  // Count the last line if it's not empty

    // Update line counter label
    gchar *lineCounterText = g_strdup_printf("Lines: %d", lineCount);
    gtk_label_set_text(lineCounter, lineCounterText);
    g_free(lineCounterText);
}

// Function to save the current file
void saveFile(const gchar *filePath) {
    GtkTextIter startIter, endIter;
    gtk_text_buffer_get_bounds(buffer, &startIter, &endIter);
    const gchar *content = gtk_text_buffer_get_text(buffer, &startIter, &endIter, FALSE);
    g_file_set_contents(filePath, content, -1, NULL);  // Save the content to the file
}

// Function to handle "Save As" dialog
void saveAs(GtkWidget *widget) {
    GtkWidget *dialog = gtk_file_chooser_dialog_new("Save As",
                                                    GTK_WINDOW(gtk_widget_get_toplevel(widget)),
                                                    GTK_FILE_CHOOSER_ACTION_SAVE,
                                                    "_Cancel", GTK_RESPONSE_CANCEL,
                                                    "_Save", GTK_RESPONSE_ACCEPT,
                                                    NULL);
    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        char *filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
        saveFile(filename);
        g_free(filename);
    }
    gtk_widget_destroy(dialog);
}

// Function to handle "Import" dialog
void importFile(GtkWidget *widget) {
    GtkWidget *dialog = gtk_file_chooser_dialog_new("Import File",
                                                    GTK_WINDOW(gtk_widget_get_toplevel(widget)),
                                                    GTK_FILE_CHOOSER_ACTION_OPEN,
                                                    "_Cancel", GTK_RESPONSE_CANCEL,
                                                    "_Open", GTK_RESPONSE_ACCEPT,
                                                    NULL);
    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        char *filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
        // Read the file content and set it to the text buffer
        gchar *content;
        g_file_get_contents(filename, &content, NULL, NULL);
        gtk_text_buffer_set_text(buffer, content, -1);
        g_free(content);
        g_free(filename);
    }
    gtk_widget_destroy(dialog);
}

// Function to paste from clipboard
void pasteText(GtkWidget *widget) {
    GtkClipboard *clipboard = gtk_clipboard_get(GDK_SELECTION_CLIPBOARD);
    GtkTextIter iter;

    // Get the current insertion point
    gtk_text_buffer_get_iter_at_mark(buffer, &iter, gtk_text_buffer_get_insert(buffer));

    // Paste the clipboard content
    gtk_text_buffer_paste_clipboard(buffer, clipboard, &iter, TRUE);
}

// Function to show the About dialog
void showAboutDialog(GtkWidget *widget) {
    GtkWidget *dialog = gtk_dialog_new_with_buttons("About",
                                                    GTK_WINDOW(gtk_widget_get_toplevel(widget)),
                                                    GTK_DIALOG_MODAL,
                                                    "_Close", GTK_RESPONSE_CLOSE,
                                                    NULL);

    GtkWidget *content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    GtkWidget *label = gtk_label_new("Created by anonp4x,\n"
                                      "a free/open source software developer.");
    gtk_container_add(GTK_CONTAINER(content_area), label);

    gtk_widget_show_all(dialog);
    
    // Clean up the dialog when closed
    g_signal_connect(dialog, "response", G_CALLBACK(gtk_widget_destroy), NULL);
}

// Function to create the text editor window
void createTextEditorWindow() {
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Text Editor");
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);

    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    // Create menu bar
    GtkWidget *menubar = gtk_menu_bar_new();
    gtk_box_pack_start(GTK_BOX(vbox), menubar, FALSE, FALSE, 0);

    // File menu
    GtkWidget *fileMenu = gtk_menu_new();
    GtkWidget *fileItem = gtk_menu_item_new_with_label("File");
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(fileItem), fileMenu);
    gtk_menu_shell_append(GTK_MENU_SHELL(menubar), fileItem);

    // Edit menu
    GtkWidget *editMenu = gtk_menu_new();
    GtkWidget *editItem = gtk_menu_item_new_with_label("Edit");
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(editItem), editMenu);
    gtk_menu_shell_append(GTK_MENU_SHELL(menubar), editItem);

    // About menu
    GtkWidget *aboutItem = gtk_menu_item_new_with_label("About");
    g_signal_connect(aboutItem, "activate", G_CALLBACK(showAboutDialog), NULL);
    gtk_menu_shell_append(GTK_MENU_SHELL(menubar), aboutItem);

    // Adding menu items
    GtkWidget *saveItem = gtk_menu_item_new_with_label("Save As");
    g_signal_connect(saveItem, "activate", G_CALLBACK(saveAs), NULL);
    gtk_menu_shell_append(GTK_MENU_SHELL(fileMenu), saveItem);

    GtkWidget *importItem = gtk_menu_item_new_with_label("Import");
    g_signal_connect(importItem, "activate", G_CALLBACK(importFile), NULL);
    gtk_menu_shell_append(GTK_MENU_SHELL(fileMenu), importItem);

    GtkWidget *pasteItem = gtk_menu_item_new_with_label("Paste");
    g_signal_connect(pasteItem, "activate", G_CALLBACK(pasteText), NULL);
    gtk_menu_shell_append(GTK_MENU_SHELL(editMenu), pasteItem);

    // Create text view and buffer
    buffer = gtk_text_buffer_new(NULL);
    textView = GTK_TEXT_VIEW(gtk_text_view_new_with_buffer(buffer));

    // Create a scrolled window to hold the text view
    GtkWidget *scrolledWindow = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolledWindow), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_container_add(GTK_CONTAINER(scrolledWindow), textView);
    
    gtk_box_pack_start(GTK_BOX(vbox), scrolledWindow, TRUE, TRUE, 0); // Pack the scrolled window into the main box

    // Create the line counter label at the bottom
    lineCounter = gtk_label_new("Lines: 0");
    gtk_box_pack_start(GTK_BOX(vbox), lineCounter, FALSE, FALSE, 0);

    // Connect the signal to update line counter on text change
    g_signal_connect(buffer, "changed", G_CALLBACK(updateLineCounter), NULL);
    
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    gtk_widget_show_all(window);
}

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);
    createTextEditorWindow();
    gtk_main();
    return 0;
}
