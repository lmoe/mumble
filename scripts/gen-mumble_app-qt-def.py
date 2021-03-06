#!/usr/bin/env python
#
# Copyright 2005-2016 The Mumble Developers. All rights reserved.
# Use of this source code is governed by a BSD-style license
# that can be found in the LICENSE file at the root of the
# Mumble source tree or at <https://www.mumble.info/LICENSE>.

# gen-mumble_app-qt-def.py creates a module definition file (.def)
# for the Qt symbols that mumble_app.dll needs to export to support
# the manual.dll plugin.
#
# The .def file is generated by running dumpbin.exe on a set of static
# Qt libraries, filtering the output to only include the symbols that
# we're interested in.
#
# The filtering process is simply a substring match on the list of
# MSVC mangled names. It might make sense to tune the filter in the
# future, since the current iteration is likely to produce unneeded
# exports.

from __future__ import (unicode_literals, print_function, division)

import subprocess
import sys
import re
import os

# qtSymbols list the Qt symbols that we're interested in exporting.
qtSymbols = [
	# Debug-build specific
	'qt_assert',
	'qt_assert_x',
	'qFlagLocation',

	# QtCore
	'QObject',
	'QCoreApplication',
	'QString',
	'QBasicAtomicInt',
	'QByteArray',
	'QListData',
	'QArrayData',

	# QtGui/QtWidgets
	'QGroupBox',
	'QGridLayout',
	'QGraphicsScene',
	'QWidget',
	'QDialog',
	'QDialogButton',
	'QLabel',
	'QLineEdit',
	'QAbstractSlider',
	'QBrush',
	'QGraphicsView',
	'QPushButton',
	'QDial',
	'QSpinBox',
	'QValidator',
	'QSpacerItem',
	'QLayoutItem',
	'QAbstractScrollArea',
	'QVBoxLayout',
	'QLayout',
	'QAbstractSpinBox',
	'QFrame',
	'QBoxLayout',
	'QDoubleSpinBox',
	'QObject',
	'QMetaObject',
	'QAbstractButton',
	'QPen',
	'QHBoxLayout',
	'QGraphicsItem',
	'QColor',
	'QGraphicsView',
	'QSizePolicy',
	'QMouseEvent',
	'QRectF',
	'QPointF',
	'QWindow',
	'QGuiApplication',
]

def processExports(f, libs, symbols):
	'''
		processExports writes a module definition file (.def) to the file object
		`f' for all symbols found in the static libraries given in `libs' that
		match any of the strings found in `symbols'.
	'''
	p = subprocess.Popen(['dumpbin.exe', '/linkermember'] + libs, stdout=subprocess.PIPE)
	stdout, stderr = p.communicate()
	if p.returncode != 0:
		raise Exception('dumpbin.exe failed: %s', stderr)

	if stdout is not None:
		stdout = stdout.decode('utf-8')
	if stderr is not None:
		stderr = stderr.decode('utf-8')

	symbol_re = re.compile('^.*\ (.*(%s)+?.*)$' % '|'.join(symbols))
	f.write('EXPORTS\n\n')

	for line in stdout.split('\r\n'):
		m = symbol_re.match(line)
		if m is not None:
			mangled, matchedsym = m.groups()
			# Qt5: avoid LNK1237
			if 'innerFunction' in mangled:
				continue
			f.write(mangled + '\r\n')

def main():
	if len(sys.argv) < 4:
		print('Usage: gen-mumble_app-qt-def.py <release|debug> <qt-lib-path> <outfn>')
		sys.exit(1)

	kind = sys.argv[1]
	qtLibDir = sys.argv[2]
	outFn = sys.argv[3]

	# Find the absolute path of the Qt libs we're interested in.
	#
	# If we're in debug mode, look for libraries with a 'd' suffix,
	# such as 'QtGuid', rather than 'QtGui'.
	#
	# Non-existant libs are skipped. This is to allow the script to
	# work on both Qt 4 and 5.
	suffix = ''
	if kind == 'debug':
		suffix = 'd'
	libs = ['QtCore', 'QtGui', 'Qt5Core', 'Qt5Gui', 'Qt5Widgets']
	libs = ['%s%s.lib' % (lib, suffix) for lib in libs]
	abslibs = [os.path.join(qtLibDir, lib) for lib in libs]
	abslibs = [lib for lib in abslibs if os.path.exists(lib)]

	with open(outFn, 'w') as f:
		processExports(f, abslibs, qtSymbols)

if __name__ == '__main__':
	main()
