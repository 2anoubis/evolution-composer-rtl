/*
 * Copyright (C) 2016 Red Hat, Inc. (www.redhat.com)
 *
 * This library is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License
 * for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library. If not, see <http://www.gnu.org/licenses/>.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <glib/gi18n-lib.h>
#include <gtk/gtk.h>
#include <webkit2/webkit2.h>

#include <e-util/e-util.h>
#include <composer/e-msg-composer.h>

#include "m-msg-composer-extension.h"

struct _MMsgComposerExtensionPrivate {
	gint dummy;
};

G_DEFINE_DYNAMIC_TYPE (MMsgComposerExtension, m_msg_composer_extension, E_TYPE_EXTENSION)

static void
set_text_direction (EContentEditor *content_editor,
                    const gchar *dir)
{
	gchar *script;

	script = g_strdup_printf (
		"document.documentElement.setAttribute('dir', '%s');"
		"document.body.style.direction = '%s';"
		"document.body.style.unicodeBidi = 'embed';",
		dir, dir);

	webkit_web_view_evaluate_javascript (
		WEBKIT_WEB_VIEW (content_editor),
		script, -1, NULL, NULL, NULL, NULL, NULL);

	g_free (script);
}

static void
action_msg_composer_ltr (EUIAction *action,
                         GVariant *value,
                         gpointer user_data)
{
	MMsgComposerExtension *ext = user_data;
	EMsgComposer *composer;
	EHTMLEditor *html_editor;
	EContentEditor *content_editor;

	g_return_if_fail (M_IS_MSG_COMPOSER_EXTENSION (ext));

	composer = E_MSG_COMPOSER (e_extension_get_extensible (E_EXTENSION (ext)));
	html_editor = e_msg_composer_get_editor (composer);
	content_editor = e_html_editor_get_content_editor (html_editor);

	set_text_direction (content_editor, "ltr");
}

static void
action_msg_composer_rtl (EUIAction *action,
                         GVariant *value,
                         gpointer user_data)
{
	MMsgComposerExtension *ext = user_data;
	EMsgComposer *composer;
	EHTMLEditor *html_editor;
	EContentEditor *content_editor;

	g_return_if_fail (M_IS_MSG_COMPOSER_EXTENSION (ext));

	composer = E_MSG_COMPOSER (e_extension_get_extensible (E_EXTENSION (ext)));
	html_editor = e_msg_composer_get_editor (composer);
	content_editor = e_html_editor_get_content_editor (html_editor);

	set_text_direction (content_editor, "rtl");
}

static const EUIActionEntry msg_composer_entries[] = {
	{ "ltr-composer-action",
	  "format-text-direction-ltr",
	  N_("Left_to_right"),
	  NULL,
	  N_("Left to right"),
	  action_msg_composer_ltr,
	  NULL, NULL, NULL },
	{ "rtl-composer-action",
	  "format-text-direction-rtl",
	  N_("Right_to_left"),
	  NULL,
	  N_("Right to left"),
	  action_msg_composer_rtl,
	  NULL, NULL, NULL }
};

static const gchar *eui_def =
	"<eui>"
	"  <toolbar id='html-toolbar'>"
	"    <item action='ltr-composer-action'/>"
	"    <item action='rtl-composer-action'/>"
	"  </toolbar>"
	"</eui>";

static void
m_msg_composer_extension_add_ui (MMsgComposerExtension *msg_composer_ext,
                                 EMsgComposer *composer)
{
	EHTMLEditor *html_editor;
	EUIManager *ui_manager;

	g_return_if_fail (M_IS_MSG_COMPOSER_EXTENSION (msg_composer_ext));
	g_return_if_fail (E_IS_MSG_COMPOSER (composer));

	html_editor = e_msg_composer_get_editor (composer);
	ui_manager = e_html_editor_get_ui_manager (html_editor);

	e_ui_manager_add_actions_with_eui_data (
		ui_manager,
		"composer-rtl",
		GETTEXT_PACKAGE,
		msg_composer_entries,
		G_N_ELEMENTS (msg_composer_entries),
		msg_composer_ext,
		eui_def);
}

static void
m_msg_composer_extension_constructed (GObject *object)
{
	EExtension *extension;
	EExtensible *extensible;

	extension = E_EXTENSION (object);
	extensible = e_extension_get_extensible (extension);

	/* Chain up to parent's constructed() method. */
	G_OBJECT_CLASS (m_msg_composer_extension_parent_class)->constructed (object);

	m_msg_composer_extension_add_ui (
		M_MSG_COMPOSER_EXTENSION (object),
		E_MSG_COMPOSER (extensible));
}

static void
m_msg_composer_extension_class_init (MMsgComposerExtensionClass *class)
{
	GObjectClass *object_class;
	EExtensionClass *extension_class;

	g_type_class_add_private (class, sizeof (MMsgComposerExtensionPrivate));

	object_class = G_OBJECT_CLASS (class);
	object_class->constructed = m_msg_composer_extension_constructed;

	/* Set the type to extend, it's supposed to implement the EExtensible interface */
	extension_class = E_EXTENSION_CLASS (class);
	extension_class->extensible_type = E_TYPE_MSG_COMPOSER;
}

static void
m_msg_composer_extension_class_finalize (MMsgComposerExtensionClass *class)
{
}

static void
m_msg_composer_extension_init (MMsgComposerExtension *msg_composer_ext)
{
	msg_composer_ext->priv = G_TYPE_INSTANCE_GET_PRIVATE (
		msg_composer_ext,
		M_TYPE_MSG_COMPOSER_EXTENSION,
		MMsgComposerExtensionPrivate);
}

void
m_msg_composer_extension_type_register (GTypeModule *type_module)
{
	m_msg_composer_extension_register_type (type_module);
}
