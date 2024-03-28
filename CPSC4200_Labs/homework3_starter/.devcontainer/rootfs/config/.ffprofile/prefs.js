// disable update-related features
user_pref('app.normandy.enabled', false);
user_pref('app.update.auto', false);
user_pref('app.update.download.promptMaxAttempts', 0);
user_pref('app.update.elevation.promptMaxAttempts', 0);

// disable welcome prompts and similar
user_pref('browser.laterrun.enabled', false);
user_pref('browser.newtabpage.enabled', false);
user_pref('browser.shell.checkDefaultBrowser', false);
user_pref('browser.startup.homepage', 'http://cpsc4200.mpese.com/');
user_pref('browser.startup.homepage_override.mstone', 'ignore');

// disable telemetry and telemetry prompt
user_pref('datareporting.healthreport.uploadEnabled', false);
user_pref('datareporting.policy.dataSubmissionEnabled', false);
user_pref(
    'toolkit.telemetry.cachedClientID',
    'c0ffeec0-ffee-c0ff-eec0-ffeec0ffeec0'
);
user_pref('toolkit.telemetry.enabled', false);
user_pref('toolkit.telemetry.rejected', true);
user_pref('toolkit.telemetry.reportingpolicy.firstRun', false);

user_pref('security.mixed_content.block_active_content', false);

// disable forms
user_pref('signon.autofillForms', false);
user_pref('signon.generation.enabled', false);
user_pref('signon.rememberSignons', false);

user_pref('browser.open.lastDir', '/workspaces/project2');
