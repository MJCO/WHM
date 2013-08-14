<?php

/**
 * @file
 * WHM Overview administrative pages.
 */

/**
 * Definition for the WHM Overview page.
 */
function whm_overview_overview_page() {
  $whmovrvw = '<h1>' . t('Web Host Manager Overview') . '</h1>';
  $srvrinfo = '<h2>' . t('Server Information') . '</h2>';
  $statushdng = '<h2>' . t('Status Information') . '</h2>';
  $output = $whmovrvw;
  $output .= '<br />';
  $output .= $srvrinfo;
  $output .= whm_overview_server_info();
  $output .= $statushdng;
  $output .= whm_overview_status_table();
  
  return $output;
}

/**
 * Build and render WHM server information table.
 */
function whm_overview_server_info() {
  $header = array(
    t('Hostname'),
    t('Main IP'),
    t('WHM Version'),
  );
  
  $rows = array();
  
  $hostname = whm_hostname();
  $version = whm_version();
  $ip = whm_main_ip();
  
  $row = array(
    array(
      $hostname,
      $ip,
      $version,
    )
  );
  
  $output = theme_table(array(
    'header' => $header,
    'rows' => $row,
    'attributes' => array(),
    // Table header will be sticky.
    'sticky' => FALSE,
    'caption' => "",
    'colgroups' => array(),
    // The message to be displayed if table is empty.
    'empty' => t('No information found. Check WHM settings.'),
    )
  );
  return $output;
}

/**
 * Build and render WHM Overview server status table.
 */
function whm_overview_status_table() {
  $header = array(
    t('Service Name'),
    t('Installed'),
    t('Enabled'),
    t('Running'),
    t('Monitored'),
    t('Service Machine Name'),
  );
  
  $rows = array();

  $srvcs = whm_get_services();

  if (!empty($srvcs)) {
    foreach ($srvcs as $srvc) {
    if ((!empty($srvc['installed']))) {
      if ($srvc['installed'] == 1) {
        $installed = t('Y');
      }
    }
    else {
      $installed = t('N');
    }
    if ((!empty($srvc['enabled']))) {
      if ($srvc['enabled'] == 1) {
        $enabled = t('Y');
      }
    }
    else {
      $enabled = t('N');
    }
    if ((!empty($srvc['running']))) {
      if ($srvc['running'] == 1) {
        $running = t('Y');
      }
    }
    else {
      $running = t('N');
    }
    if ((!empty($srvc['monitored']))) {
      if ($srvc['monitored'] == 1) {
        $monitored = t('Y');  
      } 
    }
    else {
      $monitored = t('N');
    }
    $row = array(
      $srvc['display_name'],
      $installed,
      $enabled,
      $running,
      $monitored,
      $srvc['name'],
    );
    $rows[] = $row;
  }
}
$output = theme_table(array(
  'header' => $header,
  'rows' => $rows,
  'attributes' => array(),
  // Table header will be sticky.
  'sticky' => TRUE,
  'caption' => "",
  'colgroups' => array(),
  // The message to be displayed if table is empty.
  'empty' => t('No services found. Check WHM settings.'),
  )
);

return $output;
}