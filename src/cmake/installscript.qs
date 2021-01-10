function Component()
{
}

Component.prototype.createOperations = function()
{
    component.createOperations();
    if (systemInfo.productType === "windows") {
        component.addOperation("CreateShortcut", "C:/Windows/System32/cmd.exe /A /Q /K @TargetDir@/bin/daggyenv.bat", "@StartMenuDir@/daggyenv.lnk", "iconPath=%SystemRoot%/system32/SHELL32.dll", "iconId=2",  "description=Daggy app environment");
        if(systemInfo.currentCpuArchitecture === "x86_64") {
            component.addElevatedOperation("Execute", "{0,3010,1638,5100}", "@TargetDir@/vcredist/vc_redist-x64.exe", "/norestart", "/q");
        }
    }
}

function Controller()
{
    if (installer.isInstaller())
    {
        installer.setDefaultPageVisible(QInstaller.Introduction, true);
        installer.setDefaultPageVisible(QInstaller.TargetDirectory, true);
        installer.setDefaultPageVisible(QInstaller.ComponentSelection, true);
        installer.setDefaultPageVisible(QInstaller.LicenseCheck, true);
        installer.setDefaultPageVisible(QInstaller.StartMenuSelection, true);
        installer.setDefaultPageVisible(QInstaller.ReadyForInstallation, true);
        installer.setDefaultPageVisible(QInstaller.PerformInstallation, true);
        installer.setDefaultPageVisible(QInstaller.InstallationFinished, true);
    }
}