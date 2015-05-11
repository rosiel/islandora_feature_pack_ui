/**
 * @file
 *
 * Adapted from the Henri MEDOT's ckeditor_link module by Rosie Le Faive <lefaive[at]gmail[dot]com>.
 *
 * Portions of code:
 * Copyright (c) 2003-2010, CKSource - Frederico Knabben. All rights reserved.
 * For licensing, see LICENSE.html or http://ckeditor.com/license
 */

(function($) {

    // Get a CKEDITOR.dialog.contentDefinition object by its ID.
    var getById = function(array, id, recurse) {
        for (var i = 0, item; (item = array[i]); i++) {
            if (item.id == id) return item;
            if (recurse && item[recurse]) {
                var retval = getById(item[recurse], id, recurse);
                if (retval) return retval;
            }
        }
        return null;
    };

    var resetInitValues = function(dialog) {
        dialog.foreach(function(contentObj) {
            contentObj.setInitValue && contentObj.setInitValue();
        });
    };

    var initAutocomplete = function(input, uri) {
        input.setAttribute('autocomplete', 'OFF');
        new Drupal.jsAC($(input), new Drupal.ACDB(uri));
    };

    var extractPath = function(value) {
        value = CKEDITOR.tools.trim(value);
        var match;
        match = /\(([^\(]*?)\)$/i.exec(value);
        if (match && match[1]) {
            value = 'islandora/object/' + match[1];
        }
        var basePath = Drupal.settings.basePath;
        if (value.indexOf(basePath) == 0) {
            value = value.substr(basePath.length);
        }
        if (/^[a-z][\w\/\.:-]*$/i.test(value)) {
            return value;
        }
        return false;
    };

    var cache = {}, revertPath = function(value, callback) {
        var path = extractPath(value);
        if (!path) {
            return false;
        }
        if (cache[path] !== undefined) {
            return cache[path];
        }
        $.getJSON(Drupal.settings.islandora_link.revert_path + '/' + Drupal.encodePath(path), function(data) {
            cache[path] = data;
            callback();
        });
    };

    CKEDITOR.plugins.add('islandora_path', {

        init: function(editor, pluginPath) {
            CKEDITOR.on('dialogDefinition', function(e) {
                if ((e.editor != editor) || (e.data.name != 'link') || !Drupal.settings.islandora_link) return;

                // Overrides definition.
                var definition = e.data.definition;
                definition.onFocus = CKEDITOR.tools.override(definition.onFocus, function(original) {
                    return function() {
                        original.call(this);
                        if (this.getValueOf('info', 'linkType') == 'islandora') {
                            this.getContentElement('info', 'islandora_path').select();
                        }
                    };
                });
                definition.onOk = CKEDITOR.tools.override(definition.onOk, function(original) {
                    return function() {
                        var process = false;
                        if ((this.getValueOf('info', 'linkType') == 'islandora') && !this._.selectedElement) {
                            var ranges = editor.getSelection().getRanges(true);
                            if ((ranges.length == 1) && ranges[0].collapsed) {
                                process = true;
                            }
                        }
                        original.call(this);
                        if (process) {
                            var value = this.getValueOf('info', 'islandora_path');
                            var index = value.lastIndexOf('(');
                            if (index != -1) {
                                var text = CKEDITOR.tools.trim(value.substr(0, index));
                                if (text) {
                                    CKEDITOR.plugins.link.getSelectedLink(editor).setText(text);
                                }
                            }
                        }
                    };
                });

                // Overrides linkType definition.
                var infoTab = definition.getContents('info');
                var content = getById(infoTab.elements, 'linkType');
                content.items.unshift([Drupal.settings.islandora_link.type_name, 'islandora']);
                infoTab.elements.push({
                    type: 'vbox',
                    id: 'islandoraOptions', // create new vbox element, call it islandoraOptions.
                    children: [{
                        type: 'text',
                        id: 'islandora_path',
                        label: editor.lang.link.title,
                        required: true,
                        onLoad: function() {
                            this.getInputElement().addClass('form-autocomplete');
                            initAutocomplete(this.getInputElement().$, Drupal.settings.islandora_link.autocomplete_path + 'fpp');
                        },
                        setup: function(data) {
                            this.setValue(data.islandora_path || '');
                        },
                        validate: function() {
                            var dialog = this.getDialog();
                            if (dialog.getValueOf('info', 'linkType') != 'islandora') {
                                return true;
                            }
                            var func = CKEDITOR.dialog.validate.notEmpty(editor.lang.link.noUrl);
                            if (!func.apply(this)) {
                                return false;
                            }
                            if (!extractPath(this.getValue())) {
                                alert(Drupal.settings.islandora_link.msg_invalid_path);
                                this.focus();
                                return false;
                            }
                            return true;
                        }
                    },
                        {
                            type: 'radio',
                            id: 'autocomplete_option',
                            label: 'Select a filter and autocomplete profile.',
                            items: Drupal.settings.islandora_link.autocomplete_options,
                            'default': 'group',
                            onChange: function() {
                                // this = CKEDITOR.ui.dialog.radio
                                var dialog = this.getDialog();
                                var textfield = dialog.getContentElement('info', 'islandora_path');

                                initAutocomplete(textfield.getInputElement().$, Drupal.settings.islandora_link.autocomplete_path + this.getValue());

                                //alert('Current value: ' + this.getValue());
                            }
                        }]
                });
                content.onChange = CKEDITOR.tools.override(content.onChange, function(original) {
                    return function() {
                        original.call(this);
                        var dialog = this.getDialog();
                        var element = dialog.getContentElement('info', 'islandoraOptions').getElement().getParent().getParent();
                        if (this.getValue() == 'islandora') {
                            element.show();
                            if (editor.config.linkShowTargetTab) {
                                dialog.showPage('target');
                            }
                            var uploadTab = dialog.definition.getContents('upload');
                            if (uploadTab && !uploadTab.hidden) {
                                dialog.hidePage('upload');
                            }
                        }
                        else {
                            element.hide();
                        }
                    };
                });
                content.setup = function(data) {
                    if (!data.type || (data.type == 'url') && !data.url) {
                        if (Drupal.settings.islandora_link.type_selected) {
                            data.type = 'islandora';
                        }
                    }
                    else if (data.url && !data.url.protocol && data.url.url) {
                        var dialog = this.getDialog();
                        var path = revertPath(data.url.url, function() {
                            dialog.setupContent(data);
                            resetInitValues(dialog);
                        });
                        if (path) {
                            data.type = 'islandora';
                            data.islandora_path = path;
                            delete data.url;
                        }
                    }
                    this.setValue(data.type);
                };
                content.commit = function(data) {
                    data.type = this.getValue();
                    if (data.type == 'islandora') {
                        data.type = 'url';
                        var dialog = this.getDialog();
                        dialog.setValueOf('info', 'protocol', '');
                        dialog.setValueOf('info', 'url', Drupal.settings.basePath + extractPath(dialog.getValueOf('info', 'islandora_path')));
                    }
                };
            });
        }
    });
})(jQuery);