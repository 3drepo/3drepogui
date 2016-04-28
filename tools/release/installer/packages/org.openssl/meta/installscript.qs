function Component()
{
    installer.installationFinished.connect(this, Component.prototype.installationFinishedPageIsShown);
}

Component.prototype.createOperations = function()
{
    component.createOperations();
}

Component.prototype.installationFinishedPageIsShown = function()
{
    try {
        if (installer.isInstaller() && installer.status == QInstaller.Success) {
	        QDesktopServices.openUrl("file:///" + installer.value("TargetDir") + "/Win64OpenSSL_Light-1_0_2g.exe");
        }
    } catch(e) {
        console.log(e);
    }
}

