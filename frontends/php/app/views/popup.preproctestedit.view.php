<?php
/*
** Zabbix
** Copyright (C) 2001-2019 Zabbix SIA
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
**/


$form = (new CForm())
	->cleanItems()
	->addVar('action', 'popup.preproctest.send')
	->addVar('hostid', $data['hostid'])
	->addVar('value_type', $data['value_type'])
	->addVar('test_type', $data['test_type'])
	->setId('preprocessing-test-form');

// Create macros table.
$macros_table = $data['macros'] ? new CTable() : null;
foreach ($data['macros'] as $macro_name => $macro_value) {
	$macros_table->addRow([
		(new CTextBox(null, $macro_name, true))
			->setWidth(ZBX_TEXTAREA_MACRO_WIDTH)
			->removeId(),
		'&rArr;',
		(new CTextBox('macros['.$macro_name.']', $macro_value))
			->setWidth(ZBX_TEXTAREA_MACRO_VALUE_WIDTH)
			->setAttribute('placeholder', _('value'))
			->removeId()
	]);
}

// Create results table.
$result_table = (new CTable())
	->setId('preprocessing-steps')
	->addClass('preprocessing-test-results')
	->addStyle('width: 100%;')
	->setHeader([
		'',
		(new CColHeader(_('Name')))->addStyle('width: 100%;'),
		(new CColHeader(_('Result')))->addClass(ZBX_STYLE_RIGHT)
	]);

foreach ($data['steps'] as $i => $step) {
	$form
		->addVar('steps['.$i.'][type]', $step['type'])
		->addVar('steps['.$i.'][params]', $step['params'])
		->addVar('steps['.$i.'][error_handler]', $step['error_handler'])
		->addVar('steps['.$i.'][error_handler_params]', $step['error_handler_params']);

	$result_table->addRow([
		$step['num'].':',
		(new CCol($step['name']))->setId('preproc-test-step-'.$i.'-name'),
		(new CCol())
			->addClass(ZBX_STYLE_RIGHT)
			->setId('preproc-test-step-'.$i.'-result')
	]);
}

$form_list = (new CFormList())
	->addRow(
		new CLabel(_('Value'), 'value'),
		(new CDiv([
			(new CMultilineInput('value', '', [
				'disabled' => false,
				'readonly' => false
			]))->setWidth(ZBX_TEXTAREA_STANDARD_WIDTH),
			new CLabel(_('Time'), 'time'),
			(new CTextBox(null, 'now', true))
				->setWidth(ZBX_TEXTAREA_SMALL_WIDTH)
				->setId('time')
		]))->addClass('preproc-test-popup-value-row')
	)
	->addRow(
		new CLabel(_('Previous value'), 'prev_item_value'),
		(new CDiv([
			(new CMultilineInput('prev_value', '', [
				'disabled' => !$data['show_prev']
			]))->setWidth(ZBX_TEXTAREA_STANDARD_WIDTH),
			new CLabel(_('Prev. time'), 'prev_time'),
			(new CTextBox('prev_time', $data['prev_time']))
				->setEnabled($data['show_prev'])
				->setWidth(ZBX_TEXTAREA_SMALL_WIDTH)
		]))->addClass('preproc-test-popup-value-row')
	);

if ($macros_table) {
	$form_list->addRow(
		_('Macros'),
		(new CDiv($macros_table))->addClass(ZBX_STYLE_TABLE_FORMS_SEPARATOR)
	);
}

$form_list->addRow(
	_('Preprocessing steps'),
	(new CDiv($result_table))
		->addClass(ZBX_STYLE_TABLE_FORMS_SEPARATOR)
		->addStyle('width: 100%;')
);

$form
	->addItem($form_list)
	->addItem((new CInput('submit', 'submit'))->addStyle('display: none;'));

$templates = [
	(new CTag('script', true))
		->setAttribute('type', 'text/x-jquery-tmpl')
		->setId('preprocessing-step-error-icon')
		->addItem(makeErrorIcon('#{error}')),
	(new CTag('script', true))
		->setAttribute('type', 'text/x-jquery-tmpl')
		->setId('preprocessing-gray-label')
		->addItem(
			(new CDiv('#{label}'))
				->addStyle('margin-top: 5px;')
				->addClass(ZBX_STYLE_GREY)
			),
	(new CTag('script', true))
		->setAttribute('type', 'text/x-jquery-tmpl')
		->setId('preprocessing-step-action-done')
		->addItem(
			(new CDiv([
				'#{action_name} ',
				(new CDiv(
					(new CSpan('#{failed}'))
						->addClass(ZBX_STYLE_LINK_ACTION)
						->setHint('#{failed}', '', true, 'max-width:'.ZBX_ACTIONS_POPUP_MAX_WIDTH.'px; ')
				))
					->addStyle('max-width: '.ZBX_TEXTAREA_STANDARD_WIDTH.'px;')
					->addClass(ZBX_STYLE_OVERFLOW_ELLIPSIS)
					->addClass(ZBX_STYLE_REL_CONTAINER)
			]))
				->addStyle('margin-top: 1px;')
				->addClass(ZBX_STYLE_GREY)
		)
];

$output = [
	'header' => $data['title'],
	'script_inline' => require 'app/views/popup.preproctestedit.view.js.php',
	'body' => (new CDiv([$data['errors'], $form, $templates]))->toString(),
	'buttons' => [
		[
			'title' => _('Test'),
			'class' => 'submit-test-btn',
			'keepOpen' => true,
			'isSubmit' => true,
			'action' => 'return itemPreprocessingTest("#'.$form->getId().'");'
		]
	]
];

if ($data['user']['debug_mode'] == GROUP_DEBUG_MODE_ENABLED) {
	CProfiler::getInstance()->stop();
	$output['debug'] = CProfiler::getInstance()->make()->toString();
}

echo (new CJson())->encode($output);
