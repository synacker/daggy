function Component()
{
}

Component.prototype.createOperations = function()
{
    component.createOperations();
    if (systemInfo.productType === "windows") {
        var result = QMessageBox.question("quit.question", "Daggy add to PATH", "Do you want to add Daggy to PATH environment variable?",
                                  QMessageBox.Yes | QMessageBox.No);
        if (result == QMessageBox.Yes) {
            var winpath = installer.environmentVariable("PATH") + ";" + installer.value("TargetDir") + "/bin";
            component.addElevatedOperation("EnvironmentVariable","PATH",winpath,true);
        }
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
        installer.setDefaultPageVisible(QInstaller.ComponentSelection, false);
        installer.setDefaultPageVisible(QInstaller.LicenseCheck, true);
        installer.setDefaultPageVisible(QInstaller.StartMenuSelection, true);
        installer.setDefaultPageVisible(QInstaller.ReadyForInstallation, true);
        installer.setDefaultPageVisible(QInstaller.PerformInstallation, true);
        installer.setDefaultPageVisible(QInstaller.InstallationFinished, true);
    }
}