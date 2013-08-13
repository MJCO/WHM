<?php

/**
 * @file
 * Provides API functions to cPanel's WebHost Manager (WHM) server administration software.
 * 
 * Note: Functions do not check permissions, as it can be used by other modules respecting their permissions system.
 */

class WHM_Constant {

  // error codes
  const WHM_SUCCESS =           1;   // successful return value
  const WHM_ERROR =            -1;   // generic error
  const WHM_ERROR_CONNECT =    -2;   // could not connect to WHM server
  const WHM_ERROR_AUTH =       -3;   // authentication failed
  const WHM_ERROR_CURL =       -4;   // curl not installed
  const WHM_ERROR_VARS =       -5;   // variables not specified
  const WHM_USERNAME_INVALID = -10;  // username contains invalid characters
  const WHM_USERNAME_TAKEN =   -11;  // username already taken
  const WHM_USERNAME_TOOLONG = -12;  // username is too long (max is 8 char)
  const WHM_DOMAIN_INVALID =   -20;  // domain is malformed
  const WHM_DOMAIN_TAKEN =     -21;  // domain is already taken
  const WHM_EMAIL_INVALID =    -30;  // email is invalid
  const WHM_PASSWORD_INVALID = -40;  // password is invalid

  // other constants
  const WHM_STANDARD_PORT =   2086;  // port to access WHM insecurely on
  const WHM_SECURE_PORT =     2087;  // port to access WHM securely on
  const WHM_MAX_USERNAME_LEN =   8;  // maximum user name length
  const WHM_MAX_PASSWORD_LEN =   6;  // maximum password length

}

/**
 * This function displays the server's hostname.
 */
function whm_hostname() {
  $result = _whm_command('gethostname');
  return $result['hostname'];
}

/**
 * This function displays the version of cPanel/WHM running on the server.
 */
function whm_version() {
  $result = _whm_command('version');
  return $result['version'];
}

/**
 * This function will display your server's load average. It displays the average at 3 different points in time: the 
 * average of the last minute, the last 5 minutes, and the last 15 minutes. The average value is expressed in decimal 
 * form. For example, a value of .18 would mean you were using 18% of your server's computing power.
 */
function whm_average_load() {
  // TODO depending on WHM version also call systemloadavg
  $result = _whm_command('loadavg');
  return $result;
}

/**
 * This function retrieves a list of the languages available on your server.
 */
function whm_available_languages() {
  $result = _whm_command('getlanglist');
  return $result;
}

/*************************************
 * Account functions
 * ***********************************/

/**
 * This function creates a hosting account and sets up its associated domain 
 * information.
 * 
 * @param string $user
 *   Username for the account. Example: user 
 * @param string $domain
 *   Domain name. Example: domain.tld
 * @param array $options
 *   The following variables are optional: 
 *   <li>plan (string) — Package to use for account creation. Example: reseller_gold</li> 
 *   <li>
 *     pkgname (string) — Name of a new package to be created based on the settings used. 
 *     Example: reseller_silver
 *   </li>
 *   <li>savepkg (boolean) — Save the settings used as a new package.
 *     <ul>
 *       <li>1 — yes.</li>
 *       <li>0 — no.</li>
 *     </ul>
 *   </li> 
 *   <li>
 *     featurelist (string) — Name of the feature list to be used when creating a new package. 
 *     Example: no_ftp_100mb_gold
 *   </li>
 *   <li>quota (integer) — Disk space quota in Megabytes. Allowed values:
 *     <ul>
 *       <li>A number from 0 to 999999.</li>
 *       <li>0 is unlimited.</li>
 *     </ul>
 *   </li> 
 *   <li>password (string) — Password for accessing cPanel. Example: p@ss!w0rd$123</li>
 *   <li>ip (string) — Whether or not the domain has a dedicated IP address.
 *     <ul>
 *       <li>y — yes.</li>
 *       <li>n — no.</li>
 *     </ul>
 *   </li> 
 *   <li>cgi (boolean) — Whether or not the domain has CGI access.
 *     <ul>
 *       <li>1 — yes.</li>
 *       <li>0 — no.</li>
 *     </ul>
 *   </li> 
 *   <li>frontpage (boolean) — Whether or not the domain has FrontPage extensions installed.
 *     <ul>
 *       <li>1 — yes.</li>
 *       <li>0 — no.</li>
 *     </ul>
 *   </li> 
 *   <li>hasshell (boolean) — Whether or not the domain has shell/SSH access.
 *     <ul>
 *       <li>1 — yes.</li>
 *       <li>0 — no.</li>
 *     </ul>
 *   </li> 
 *   <li>contactemail (string) — Contact email address for the account. Example: user@otherdomain.tld</li>
 *   <li>cpmod (string) — cPanel theme name. Example: x3</li>
 *   <li>maxftp (string) — Maximum number of FTP accounts the user can create. Allowed values:
 *     <ul>
 *       <li>A number from 0 to 999999.</li>
 *       <li>0 is unlimited.</li>
 *       <li>unlimited</li>
 *       <li>null</li>
 *     </ul>
 *   </li> 
 *   <li>maxsql (string) — Maximum number of SQL databases the user can create. Allowed values:
 *     <ul>
 *       <li>A number from 0 to 999999.</li>
 *       <li>0 is unlimited.</li>
 *       <li>unlimited</li>
 *       <li>null</li>
 *     </ul>
 *   </li>
 *   <li>maxpop (string) — Maximum number of email accounts the user can create. Allowed values:
 *     <ul>
 *       <li>A number from 0 to 999999.</li>
 *       <li>0 is unlimited.</li>
 *       <li>unlimited</li>
 *       <li>null</li>
 *     </ul>
 *   </li> 
 *   <li>maxlst (string) — Maximum number of mailing lists the user can create. Allowed values:
 *     <ul>
 *       <li>A number from 0 to 999999.</li>
 *       <li>0 is unlimited.</li>
 *       <li>unlimited</li>
 *       <li>null</li>
 *     </ul>
 *   </li> 
 *   <li>maxsub (string) — Maximum number of subdomains the user can create. Allowed values:
 *     <ul>
 *       <li>A number from 0 to 999999.</li>
 *       <li>0 is unlimited.</li>
 *       <li>unlimited</li>
 *       <li>null</li>
 *     </ul>
 *   </li> 
 *   <li>maxpark (string) — Maximum number of parked domains the user can create. Allowed values:
 *     <ul>
 *       <li>A number from 0 to 999999.</li>
 *       <li>0 is unlimited.</li>
 *       <li>unlimited</li>
 *       <li>null</li>
 *     </ul>
 *   </li> 
 *   <li>maxaddon (string) — Maximum number of addon domains the user can create. Allowed values:
 *     <ul>
 *       <li>A number from 0 to 999999.</li>
 *       <li>0 is unlimited.</li>
 *       <li>unlimited</li>
 *       <li>null</li>
 *     </ul>
 *   </li> 
 *   <li>bwlimit (string) — Bandiwdth limit in Megabytes. Must be a number from 0 to 999999.
 *     <ul>
 *       <li>0 is unlimited.</li>
 *     </ul> 
 *   </li>
 *   <li>customip (string) — Specific IP address for the site.</li>
 *   <li>language (string) — Language to use in the account's cPanel interface. Example: spanish-utf8</li>
 *   <li>
 *     useregns (boolean) — Use the registered nameservers for the domain instead of the ones configured on the 
 *     server.
 *     <ul>
 *       <li>1 — yes.</li>
 *       <li>0 — no.</li>
 *     </ul>
 *   </li> 
 *   <li>hasuseregns (boolean) — Set to 1 if you are using the above option.</li>
 *   <li>reseller (boolean) — Give reseller privileges to the account.
 *     <ul>
 *       <li>1 — yes.</li>
 *       <li>0 — no.</li>
 *     </ul>
 *   </li> 
 *   <li>forcedns (boolean) — Overwrite current DNS Zone if a DNS Zone already exists.
 *     <ul>
 *       <li>1 — yes.</li>
 *       <li>0 — no.</li>
 *     </ul>
 *   </li> 
 *   <li>
 *     mxcheck — Determines how the server will handle incoming mail for this domain. This variable can be set to 
 *     one of the following: 
 *     <ul>
 *       <li>
 *         local — The domain will accept mail, regardless of whether a higher-priority mail exchanger has been 
 *         designated on the WHM Edit MX Entry screen. (If a higher-priority mail exchanger exists, mail will be 
 *         routed to both domains.)
 *       </li>
 *       <li>
 *         secondary (or backup) — The domain will act as a backup mail exchanger, holding mail in queue if the 
 *         primary exchanger becomes unavailable. 
 *         Note: You will still need to use the WHM Edit MX Entry screen to configure the primary MX entry to point to 
 *         the appropriate exchanger.
 *       </li>
 *       <li> 
 *         remote — The domain will not accept mail, instead sending it to the primary mail exchanger. 
 *         Note: You will still need to use the WHM Edit MX Entry screen to configure the primary MX entry to point to 
 *         the appropriate exchanger.
 *       </li>
 *       <li>
 *         auto — The server will automatically detect, and use, the configuration set on the WHM Edit MX Entry screen.
 *       </li>
 *     </ul>
 *   </li>
 */
function whm_create_account($user, $domain, $options = array()) {
  $rv = whm_check_username($user);
  if ($rv !== WHM_Constant::WHM_SUCCESS) {
    return $rv;
  }
  $rv = whm_check_domain($domain);
  if ($rv !== WHM_Constant::WHM_SUCCESS) {
    return $rv;
  }
  if ($options['contactemail']) {
    $rv = whm_check_email($options['contactemail']);
    if ($rv !== WHM_Constant::WHM_SUCCESS) {
      return $rv;
    }
  }
  if ($options['pass']) {
    $rv = whm_check_password($options['pass']);
    if ($rv !== WHM_Constant::WHM_SUCCESS) {
      return $rv;
    }
  }
  if ($options['plan']) {
    $rv = whm_check_plan($options['plan']);
    if ($rv !== WHM_Constant::WHM_SUCCESS) {
      return $rv;
    }
  }

  $options = array(
    'username' => $user,
    'domain' => $domain,
  ) + $options;

  $result = _whm_command('createacct', $options);
  if ($result['result'][0]['status'] == 1) {
    return WHM_Constant::WHM_SUCCESS;
  }
  return t($result['result'][0]['statusmsg']);
}

/**
 * This function changes the password of a domain owner (cPanel) or reseller (WHM) account.
 * 
 * @param string $user
 *   Name of the user whose password should be changed.
 * @param string $pass
 *   New password for the user.
 */
function whm_change_password($user, $pass) {
  $rv = whm_check_password($pass);
  if ($rv === WHM_Constant::WHM_SUCCESS) {
    $result = _whm_command('passwd', array('user' => $user, 'pass' => $pass));
    if ($result['passwd'][0]['status'] == 1) {
      return WHM_Constant::WHM_SUCCESS;
    }
  } 
  else {
    return $rv;
  }
  return WHM_Constant::WHM_ERROR;
}

/**
 * 
 * This function modifies the bandwidth usage (transfer) limit for a specific account.
 *  
 * @param string $user
 *   Name of the user for whom you wish to modify the bandwidth usage limit.
 * @param string $limit
 *   Bandwidth usage limit, in Megabytes.
 */
function whm_limit_bandwidth($user, $limit) {
  $result = _whm_command('limitbw', array('user' => $user, 'bwlimit' => $limit));
  if ($result['result'][0]['status'] == 1) {
    return WHM_Constant::WHM_SUCCESS;
  }
  return WHM_Constant::WHM_ERROR;
}

/**
 * This function lists all accounts on the server, and also allows you to  search for a specific account or set of 
 * accounts.
 * 
 * @param string $search_type
 *   Type of account search. Allowed values: domain, owner, user, ip, package
 * @param string $search
 *   Search criteria; this field takes regular expressions.
 */
function whm_list_accounts($search_type = NULL, $search = NULL) {
  $opts = array();
  if ($search_type != NULL) {
    $opts['searchtype'] = $search_type;
  }
  if ($search != NULL) {
    $opts['search'] = $search;
  }
  $result = _whm_command('listaccts', $opts);
  return $result['acct'];
}

/**
 * This function modifies settings for an account.
 *  
 * <b>Important:</b> Instead of using this function call, we recommend changing the account's package with the 
 * Upgrade/Downgrade an Account feature. When you modify an account using this API function, the changes are in 
 * danger of being overwritten when the account's package is modified.
 * 
 * @param $user
 *   User name of the account. Example: user
 * @param array $options
 *   The following variables are optional: 
 *   <li>domain (string) — Domain name. Example: domain.tld</li>
 *   <li>newuser (string) — Used when changing the username of an account. This will be the new username.</li> 
 *   <li>owner (string) — Change the owner of the account to the specified owner.</li>
 *   <li>CPTHEME (string) — cPanel theme name. Example: x3</li>
 *   <li>HASCGI (boolean) — Whether or not the domain has CGI access.
 *     <ul>
 *       <li>1 — yes.</li>
 *       <li>0 — no.</li>
 *     </ul>
 *   </li>
 *   <li>LANG (string) — Language to use in the account's cPanel interface. Example: spanish-utf8</li> 
 *   <li>
 *     LOCALE (string) — Locale to use in the account's cPanel interface. Example: en 
 *     Note: This input variable is only compatible with WHM version 11.25 and later.
 *   </li> 
 *   <li>MAXFTP (string) — Maximum number of FTP accounts the user can create. Allowed values:
 *     <ul>
 *       <li>A number from 0 to 999999. 0 is unlimited.</li> 
 *       <li>unlimited</li> 
 *       <li>null</li>
 *     </ul>
 *   </li>
 *   <li>MAXSQL (string) — Maximum number of SQL databases the user can create. Allowed values: 
 *     <ul>
 *       <li>A number from 0 to 999999. 0 is unlimited.</li> 
 *       <li>unlimited</li> 
 *       <li>null</li>
 *     </ul>
 *   </li>
 *   <li>MAXPOP (string) — Maximum number of email accounts the user can create. Allowed values: 
 *     <ul>
 *       <li>A number from 0 to 999999. 0 is unlimited.</li> 
 *       <li>unlimited</li> 
 *       <li>null</li>
 *     </ul>
 *   </li>
 *   <li>MAXLST (string) — Maximum number of mailing lists the user can create. Allowed values: 
 *     <ul>
 *       <li>A number from 0 to 999999. 0 is unlimited.</li> 
 *       <li>unlimited</li> 
 *       <li>null</li>
 *     </ul>
 *   </li>
 *   <li>MAXSUB (string) — Maximum number of subdomains the user can create. Allowed values:
 *     <ul>
 *       <li>A number from 0 to 999999. 0 is unlimited.</li> 
 *       <li>unlimited</li> 
 *       <li>null</li>
 *     </ul>
 *   </li>
 *   <li>MAXPARK (string) — Maximum number of parked domains the user can create. Allowed values: 
 *     <ul>
 *       <li>A number from 0 to 999999. 0 is unlimited.</li> 
 *       <li>unlimited</li> 
 *       <li>null</li>
 *     </ul>
 *   </li>
 *   <li>MAXADDON (string) — Maximum number of addon domains the user can create. Allowed values: 
 *     <ul>
 *       <li>A number from 0 to 999999. 0 is unlimited.</li> 
 *       <li>unlimited</li> 
 *       <li>null</li>
 *     </ul>
 *   </li>
 *   <li>shell (boolean) — Whether or not the domain has shell/SSH access. 
 *     <ul>
 *       <li>1 — yes.</li>
 *       <li>0 — no.</li>
 *     </ul>
 *   </li>
 */
function whm_modify_account($user, $options = array()) {
  $rv = whm_check_username($options['newuser']);
  if ($rv !== WHM_Constant::WHM_SUCCESS) {
    return $rv;
  }

  $options = array(
    'user' => $user,
  ) + $options;

  $result = _whm_command('modifyacct', $options);
  if ($result['result'][0]['status'] == 1) {
    return WHM_Constant::WHM_SUCCESS;
  }
  return t($result['result'][0]['statusmsg']);
}

/**
 * This function changes an account's disk space usage quota.
 * 
 * @param string $user
 *   User name of the account. Example: user
 * @param integer $quota
 *   The user's new disk space quota, in Megabytes. Example: 500
 */
function whm_modify_quota($user, $quota) {
  $result = _whm_command('editquota', array('user' => $user, 'quota' => $quota));
  if ($result['result'][0]['status'] == 1) {
    return WHM_Constant::WHM_SUCCESS;
  }
  return WHM_Constant::WHM_ERROR;
}

/**
 * This function displays pertinent information about a specific account.
 * 
 * @param string $user
 *   Username associated with the acount you wish to display.
 */
function whm_account_info($user) {
  $result = _whm_command('accountsummary', array('user' => $user));
  if ($result['status'] == 1) {
    return $result['acct'];
  }
  return WHM_Constant::WHM_ERROR;
}

/**
 * Suspend user from current reseller account.
 */
function whm_suspend_account($user) {
  if (empty($user)) {
    return WHM_Constant::WHM_ERROR_VARS;
  }
  $user = urlencode($user);
  $result = _whm_command('suspendacct', array('user' => $user));
  if ($result['result'][0]['status'] == 1) {
    return WHM_Constant::WHM_SUCCESS;
  }
  return WHM_Constant::WHM_ERROR;
}

/**
 * Terminates a WHM account from current reseller account.
 */
function whm_terminate_account($user) {
  if (empty($user)) {
    return WHM_Constant::WHM_ERROR_VARS;
  }
  $user = urlencode($user);
  $result = _whm_command('removeacct', array('user' => $user));
  if ($result['result'][0]['status'] == 1) {
    return WHM_Constant::WHM_SUCCESS;
  }
  return WHM_Constant::WHM_ERROR;
}

/**
 * Unsuspend user from current reseller account.
 */
function whm_unsuspend_account($user) {
  if (empty($user)) {
    return WHM_Constant::WHM_ERROR_VARS;
  }
  $user = urlencode($user);
  $result = _whm_command('unsuspendacct', array('user' => $user));
  if ($result['result'][0]['status'] == 1) {
    return WHM_Constant::WHM_SUCCESS;
  }
  return WHM_Constant::WHM_ERROR;
}

/**
 * Check availability of username from listaccts (only for current reseller).
 */
function whm_check_username($user) {
  $user = drupal_strtolower(trim($user));
  
  // check if username is too long
  if (drupal_strlen($user) > WHM_Constant::WHM_MAX_USERNAME_LEN) {
    return WHM_Constant::WHM_USERNAME_TOOLONG;
  }

  // check username for non-alphanumeric characters
  $validchr = 'abcdefghijklmnopqrstuvwxyz0123456789';
  for ($i=0; $i<drupal_strlen($user); $i++) {
    if (strpos($validchr, $user[$i])===FALSE) {
      return WHM_Constant::WHM_USERNAME_INVALID;
    }
  }
  
  // check if username is already taken
  $accts = whm_list_accounts();
  foreach ($accts as $acct) {
    if ($acct['user'] == $user) {
      return WHM_Constant::WHM_USERNAME_TAKEN;
    }
  }
  return WHM_Constant::WHM_SUCCESS;
}

/**
 * Check availability of domain from listaccts (only for current reseller).
 */
function whm_check_domain($domain) {
  $domain = drupal_strtolower(trim($domain));
  
  // check domain for valid length (between 2 and 63)
  if ( (drupal_strlen($domain)<2) || (drupal_strlen($domain)>63) ) {
    return WHM_Constant::WHM_DOMAIN_INVALID;
  }
  
  // check domain for valid characters
  $validchr = 'abcdefghijklmnopqrstuvwxyz0123456789-.';
  for ($i=0; $i<drupal_strlen($domain); $i++) {
    if (strpos($validchr, $domain[$i]) === FALSE) {
      return WHM_Constant::WHM_DOMAIN_INVALID;
    }
  }

  // check that domain has at least one period
  $dot = strpos($domain, '.');
  if ($dot === FALSE) {
    return WHM_Constant::WHM_DOMAIN_INVALID;
  }
  
  // check that domain does not begin or end with a hyphen or period
  if ( ($domain[0]=='.') || ($domain[0]=='-') || ($domain[$dot-1]=='.') || ($domain[$dot-1]=='-') ) {
    return WHM_Constant::WHM_DOMAIN_INVALID;
  }
  
  // if domain begins with 'www.' strip subdomain
  if (drupal_substr($domain, 0, 4) === "www.") {
    $domain = drupal_substr($domain, 4);
  }
  
  // check if domain is already taken
  $accts = whm_list_accounts();
  foreach ($accts as $acct) {
    if ($acct['domain'] == $domain) {
      return WHM_Constant::WHM_DOMAIN_TAKEN;
    }
  }
  return WHM_Constant::WHM_SUCCESS;
}

/**
 * Check e-mail address for proper format and characters.
 */
function whm_check_email($mail) {
  $mail = drupal_strtolower(trim($mail));
  $pos = strpos($mail, '@');
  if ( ($pos===FALSE) || ($pos===0) ) {
    return WHM_Constant::WHM_EMAIL_INVALID;
  }

  $user = drupal_substr($mail, 0, $pos);
  $validchr = "abcdefghijklmnopqrstuvwxyz0123456789!#$%^&*-=_+?{}|~.";
  for ($i=0; $i<drupal_strlen($user); $i++) {
    if (strpos($validchr, $user[$i])===FALSE) {
      return WHM_Constant::WHM_EMAIL_INVALID;
    }
  }
  if ( ($user[0]=='.') || ($user[drupal_strlen($user)-1]=='.') ) {
    return WHM_Constant::WHM_EMAIL_INVALID;
  }

  $domain = drupal_substr($mail, $pos+1);
  if (whm_check_domain($domain) !== WHM_Constant::WHM_SUCCESS) {
    return WHM_Constant::WHM_EMAIL_INVALID;
  }
  return WHM_Constant::WHM_SUCCESS;
}

/**
 * Check password for allowed characters.
 */
function whm_check_password($pass) {
  if (drupal_strlen($pass) < WHM_Constant::WHM_MAX_PASSWORD_LEN) {
    return WHM_Constant::WHM_PASSWORD_INVALID;
  }
  return WHM_Constant::WHM_SUCCESS;
}

/**
 * Check user-specified plan type with actual plan choices.
 */
function whm_check_plan($plan) {
  $plan=trim($plan);
  $plans = whm_list_plans();
  foreach ($plans as $plan_data) {
    if ($plan_data['name'] == $plan) {
      return WHM_Constant::WHM_SUCCESS;
      break;
    }
  }
  return WHM_Constant::WHM_ERROR;
}

/**
 * This function lists all hosting packages available for use by the WHM user who is currently logged in. If that user 
 * is a reseller, he or she may not see some packages that exist, if those packages are not currently available to be 
 * used for account creation.
 * 
 */
function whm_list_plans() {
  $data = _whm_command('listpkgs');
  return $data['package'];
}

function whm_plan_info($plan) {
  $data = _whm_command('listpkgs');
  $results = $data['package'];
  foreach ($results as $result) {
    if ($result['name'] = $plan) {
      return $result;
    }
  }
  return WHM_Constant::WHM_ERROR;
}

function whm_create_plan($name, $options = array()) {
  $options = array(
    'name' => $name,
  ) + $options;
  $result = _whm_command('addpkg', $options);
  if ($result['result'][0]['status'] == 1) {
    return WHM_Constant::WHM_SUCCESS;
  }
  return t($result['result'][0]['statusmsg']);
}

function whm_modify_plan($name, $options = array()) {
  $options = array(
    'name' => $name,
  ) + $options;
  $result = _whm_command('editpkg', $options);
  if ($result['result'][0]['status'] == 1) {
    return WHM_Constant::WHM_SUCCESS;
  }
  return t($result['result'][0]['statusmsg']);
}

function whm_delete_plan($plan_name) {
  if (empty($plan_name)) {
    return WHM_Constant::WHM_ERROR_VARS;
  }
  $result = _whm_command('killpkg', array('pkg' => urlencode($plan_name)));
  if ($result['result'][0]['status'] == 1) {
    return WHM_Constant::WHM_SUCCESS;
  }
  return WHM_Constant::WHM_ERROR;
}

/**
 * Get array of possible plan types from WHM.
 */
function whm_list_feature_lists() {
  $data = _whm_command('getfeaturelist');
  $plans = $data['package'];
  $plan_names = array();
  foreach ($plans as $plan) {
    $plan_names[] = $plan['name'];
  }
  return $plan_names;
}

/**
 * Execute WHM script as current reseller account.
 * 
 * 
 * Returns error code or response data.
 * 
 * @param $command
 *   The name of the WHM command resource to resolve.
 * @param $args
 *   Any optional parameters that maybe passed as args to the target WHM 
 *   command.
 */
function _whm_command($command, $args = array()) {
  $host = variable_get('whm_hostname', '');
  $whmuser = variable_get('whm_username', '');
  $whmauthkey = variable_get('whm_authkey', '');

  if (empty($host) || empty($whmuser) || empty($whmauthkey)) {
    return WHM_Constant::WHM_ERROR_VARS;
  }

  $headers = _whm_build_command_headers();
  $url = _whm_build_command_url($command, $args);

  watchdog('whm', "Attempting to resolve $url.", NULL, WATCHDOG_DEBUG);
  watchdog('whm', 'Auth Header: ' . $headers['Authorization'], NULL, WATCHDOG_DEBUG);

  $response = drupal_http_request($url, $options = array('headers' => $headers));
  watchdog('whm', "Resolving URL $url returned status code $response->code.", NULL, WATCHDOG_DEBUG);
  if ($response->code == "401") {
    return WHM_Constant::WHM_ERROR_AUTH;
  }
  else {
    $data = json_decode($response->data, TRUE);
  }

  return $data;
}

function _whm_build_command_url($command, $args = array()) {
  $host = variable_get('whm_hostname', '');
  $port = variable_get('whm_port', WHM_Constant::WHM_STANDARD_PORT);
  $protocol = $port == WHM_Constant::WHM_SECURE_PORT ? 'https' : 'http';
  $url = "$protocol://$host:$port/json-api/$command";
  if (count($args) > 0) {
    $url .= '?';
    foreach ($args as $key => $value) {
      $url .= $key . '=' . $value . '&';
    }
    $url = substr_replace($url, "", -1);
  }
  return $url;
}

function _whm_build_command_headers() {
  $host = variable_get('whm_hostname', '');
  $whmuser = variable_get('whm_username', '');
  $whmpass = variable_get('whm_password', '');
  $whmauthkey = variable_get('whm_authkey', '');
  $auth_type = 'whm';
  $headers = array();
  if ($auth_type == 'whm') {
    $headers['Authorization'] = "WHM $whmuser:" . preg_replace("'(\r|\n)'", "", $whmauthkey);
  }
  else {
    $headers['Authorization'] = 'Basic ' . base64_encode($whmuser . ":" . $whmpass);
  }
  return $headers;
}

function _whm_theme_plan_name($name) {
  $whmuser = variable_get('whm_username', '');
  return str_replace($whmuser . '_', '', $name);  
}
