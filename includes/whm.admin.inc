<?php

/**
 * @file
 * WHM administrative forms.
 */

_whm_include('whm.api');

/**
 * TODO: Implement this function.
 */
function whm_admin_overview() {
}

/**
 * Definition for the settings form.
 */
function whm_admin_settings() {

  $form['host'] = array(
    '#type' => 'fieldset',
    '#title' => t('WHM Host Settings'),
  );
  $form['host']['hostname'] = array(
    '#type' => 'textfield',
    '#title' => t('WHM Server Host'),
    '#description' => t('The WHM server (IP address or hostname, hostname may not work on all systems depending on server DNS config.).'),
    '#default_value' => variable_get('whm_hostname', ''),
    '#required' => TRUE,
    '#size' => 32,
  );
  $form['host']['port'] = array(
    '#type' => 'radios',
    '#title' => t('WHM Connection Port'),
    '#description' => t('Port/Security setting for communication with the WHM server.'),
    '#default_value' => variable_get('whm_port', 2086),
    '#required' => TRUE,
    '#options' => array(
      '2086' => t('Insecure (Port: 2086)'),
      '2087' => t('Secure (Port: 2087)'),
    ),
  );
  $form['host']['reseller'] = array(
    '#type' => 'checkbox',
    '#title' => t('This is a reseller account'),
    '#description' => t('Not implemented yet.'),
    '#default_value' => variable_get('whm_reseller', FALSE),
  );
  $form['whm'] = array(
    '#type' => 'fieldset',
    '#title' => t('WHM Access Settings'),
  );
  $form['whm']['user'] = array(
    '#type' => 'textfield',
    '#title' => t('WHM Username'),
    '#description' => t('Enter WHM username for reseller account to use.'),
    '#default_value' => variable_get('whm_username', ''),
    '#required' => TRUE,
    '#size' => 24,
  );
  $form['whm']['auth'] = array(
    '#type' => 'textarea',
    '#title' => t('WHM Remote Access Key'),
    '#description' => t('Enter WHM remote access key for the WHM installation. This is found in WHM.'),
    '#default_value' => variable_get('whm_authkey', ''),
    '#required' => TRUE,
  );

  $form['#validate'][] = 'whm_admin_settings_validate';
  $form['#submit'][] = 'whm_admin_settings_submit';

  return system_settings_form($form);
}

/**
 * Validation for the settings form.
 */
function whm_admin_settings_validate($form, &$form_state) {
  if (empty($form_state['values']['hostname'])) {
    form_set_error('hostname', t('Please enter the hostname of the WHM server.'));
  }

  if (empty($form_state['values']['user'])) {
    form_set_error('user', t('Please enter the WHM username to use.'));
  }
  elseif (drupal_strlen($form_state['values']['user']) > WHMConstant::WHM_MAX_USERNAME_LEN) {
    form_set_error('user', t('Username length must be less than or equal to %len', array('%len' => WHMConstant::WHM_MAX_USERNAME_LEN)));
  }

  if (empty($form_state['values']['auth'])) {
    form_set_error('auth', t('Please enter the WHM remote access key.'));
  }

  if ($form_state['values']['port'] == 2086) {
    drupal_set_message(t('You have set WHM to use an insecure port, this is not recommended but is allowed.'), 'warning');
  }

  if ($form_state['values']['ignoresscert'] == 1) {
    drupal_set_message(t('You have set WHM to ignore self signed certificates, this is not recommended but is allowed.'), 'warning');
  }
}

/**
 * Submit processing for the settings form.
 */
function whm_admin_settings_submit($form, &$form_state) {

  variable_set('whm_hostname', trim($form_state['values']['hostname']));
  variable_set('whm_username', trim($form_state['values']['user']));
  variable_set('whm_authkey', trim($form_state['values']['auth']));
  variable_set('whm_port', trim($form_state['values']['port']));
  variable_set('whm_ignoresscert', trim($form_state['value']['port']));

  // Test connection by listing accounts.
  $result = whm_version();
  if ($result === WHMConstant::WHM_ERROR_CONNECT) {
    form_set_error('hostname', t('Unable to connect to WHM. Please check the hostname.'));
  }
  elseif ($result === WHMConstant::WHM_ERROR_AUTH) {
    form_set_error('user', t('Unable to authenticate with WHM. Please check the username.'));
    form_set_error('auth', t('Unable to authenticate with WHM. Please check the remote access key that should be copied from WHM.'));
  }
  elseif ($result == WHMConstant::WHM_ERROR_CURL) {
    drupal_set_message(t('cURL does not appear to be setup on this server. Please ensure that your PHP installation has cURL setup properly.'), 'error');
  }
  elseif (!empty($result)) {
    drupal_set_message(t('A connection to WHM @%host has been successfully established.', array('%host' => $result)));
  }
  else {
    drupal_set_message(t('Unable to verify connection with WHM. Please check the settings below, and ensure that WHM is configured to allow remote access.'), 'error');
  }
}
