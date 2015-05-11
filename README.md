# Islandora UI Feature Pack 

## Introduction

This module implements a few features that Islandora admins might find useful.

## Requirements

This module requires the following modules/libraries:

* [Islandora](https://github.com/islandora/islandora)

Additionally, installing and enabling [CKEditor](https://www.drupal.org/project/ckeditor)
will allow you to create auto-complete-able links to Islandora content in the CKEditor WYSIWYG.


# Installation

Install as usual, see [this](https://drupal.org/documentation/install/modules-themes/modules-7) for further information.

To enable the CKEditor "Islandora autocomplete" plugin, go to Configuration -> Content authoring -> CKEditor (/admin/config/content/ckeditor), _edit_ the desired profile, and under _Editor Appearance_ enable the "Islandora Link" plugin. You will then need to configure at least one autocomplete configuration.

## Configuration

Configure autocomplete profiles at Admin -> Islandora -> Islandora Utility Modules -> User Interface Features (/admin/islandora/tools/ui). 

### Features

* "Add DSID" and "Edit DSID" tabs will show up on all islandora objects that have associations with XML forms for editing.
* An autocomplete link tab is added to CKEditor's link plugin to easily add links to islandora objects. 


## Troubleshooting/Issues

This isn't officially maintained yet; if you have trouble send me a message lefaive [at] gmail [dot] com.

## Maintainers/Sponsors

Current maintainers:

* [Rosie Le Faive](https://github.com/rosiel)

## Development

in progress.

## License

[GPLv3](http://www.gnu.org/licenses/gpl-3.0.txt)
