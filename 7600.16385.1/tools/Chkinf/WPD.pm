# Copyright 1999-2000 Microsoft Corporation

# The name of this module. For our purposes it must be all in caps.
package WPD;

    use strict;          # Keep the code clean
    my $DEBUG  = 0;      # Set to 1 for debugging output

    #---------------------------------------------------#
    # Module level variables
    #---------------------------------------------------#
    my $Version= "5.1.2402.0"; # Version of this file
    my $Current_INF_File ;     # Stores name of the current INF file.
    my $Current_HTM_File ;     # Name of the output file to use.

    my(@Device_Class_Options); # Array to store device specific options.

    my(%AllModels);            # Hash of all models present in INF

    my(%ErrorTable) = (1999 => [" ERROR NUMS < 2000 ARE RESERVED "],
                       2001 => ["The value of %s should be %s."],
                       2002 => ["Section [%s] could not be found."],
                       2003 => ["%s is expected to %s."],
                       2004 => ["Too many values for directive %s"],
                       2005 => ["Unrecognized directive: %s"],
                       2006 => ["Reg-root %s should be specified for '%s' registry key."],
                       2007 => ["Illegal value for '%s' registry key"],
                       2008 => ["Flag %s must be specified for '%s' registry key."],
                       2009 => ["Required registry key '%s' is not present in any of the registry sections"],
                       2010 => ["Required directive '%s' could not be found."],
                       2011 => ["Illegal value for directive '%s'"],
                       2012 => ["%s should be in the form '%s'"],
                       2013 => ["Service name '%s' has not been defined using the UmdfService directive"],
                       2014 => ["Service '%s' has not been specified in the UmdfServiceOrder directive"],
                       2015 => ["Malformed GUID specified in DriverCLSID value"],
                       2016 => ["'%s' must be listed as one of the co-installers"],
                       2017 => ["Duplicate instance found for the '%s' directive"],
                       9999 => [" DO NOT USE THIS ERROR NUM "]);

    my(%WarningTable)=(1999 => [" WARNING NUMS < 2000 ARE RESERVED "],
                       3001 => ["\"VendorSetup\" may be included it for backward compatibility with Windows 98, but it has no effect on Windows 2000."],
                       3002 => ["Service binary should be copied and run from %%windir\%\\system32\\drivers\\UMDF directory (Use %%12\%\\UMDF)."],
                       3003 => ["Do not copy file %s directly, use %s in the [%s] section."],
                       3004 => ["Duplicate instance found for '%s' registry entry"],
                       3005 => ["INF should use WUDF Redirector (%%12\%\\WUDFRd.sys) as the service"],
                       3006 => ["Value for directive '%s' should be set to %s"],
                       9999 => [" DO NOT USE THIS WARNING NUM "]);

    #--- Helper Subroutines ---#
    sub TRUE  { return(1); }
    sub FALSE { return(0); }

#-------------------------------------------------------------------------------------------------#
# Verify that WUDFCoInstaller.dll or WudfUpdate_\d+.dll is listed as one of the co-installers     #
#-- CheckWDFCoInstaller($Model) ------------------------------------------------------------------#
sub CheckWDFCoInstaller {
    my $line;
    my $count = 0;
    my $Model    = $_;
       $Model    = uc($Model);
    my $Section = $Model.".CoInstallers";

    my($Directive, @Values, $Value, $Temp);

    # INF must have a [DDInstall.Coinstallers] for each [DDInstall] section
    unless ($Current_INF_File->sectionDefined($Section)) {   
        AddError(1, 2002, $Section);
        return;
    }

    # Check if the correct system provided INF section has been included
    # If so, we may skip checking for an explicit CoInstallers registry entry
    my $IncludeValue = "wpdmtp.inf";
    my $NeedsValue = "WPD.MTP.CoInstallers";
    
    if (is_defined($Section, "INCLUDE", $IncludeValue) &&
        is_defined($Section, "NEEDS", $NeedsValue)) 
    {   
        # Skip the rest of the checks
        return;
    }                            
    
    # RequiredRegDirectives is a map of the registry entry with values for the 
    # required reg root, reg subroot, type, value, found flag
    my (%RequiredRegDirectives) = ('COINSTALLERS32'            => ["HKR", "", "0x10000", "\^WUDF(COINSTALLER|UPDATE_\\d+)\.DLL\$", 0],
                                   );

    my @lines = $Current_INF_File->getSection($Section);    
    
    foreach $line (@lines) {
        $count++, next if ($line =~ /^(\s*|;.*)$/);# Skip if it's a blank line
        
        ($Directive,$Value) = ChkInf::SplitLine($Section,$count,$line);

        if (uc $Directive eq "ADDREG") {
            my @RegSections = split /,/,$Value;
            my $RegSection;
            
            foreach $RegSection (@RegSections) {
                $RegSection =~ s/^\s*//i; # Kill leading spaces
                $RegSection =~ s/\s*$//i; # Kill trailing spaces
                
                if ( $Current_INF_File->sectionDefined($RegSection) ) {
                    my @RegSectionLines = $Current_INF_File->getSection($RegSection);
                    my $regcount = 0;
                    my $regline;
                    
                    foreach $regline (@RegSectionLines) {
                        $regcount++, next if ($regline =~ /^(\s*|;.*)$/);# Skip if it's a blank line
                        
                        my @regparts = split /,/,$regline;
                        
                        $regcount++, next if ($regparts[2] =~ /^(\s*|;.*)$/);# Skip if it's a blank entry
                        
                        $regparts[2]    =~  s/^\"(.*)\"/$1/;   # And Strip the Quotes!
                
                        if (defined($RequiredRegDirectives{uc $regparts[2]})) {

                            # If we've already found this directive before, flag it as a warning
                            AddWarning($Current_INF_File->getIndex($RegSection, $regcount), 3004, $regparts[2])
                                    if ($RequiredRegDirectives{uc $regparts[2]}[4] == 1);
                            
                            # Mark that we found the directive
                            $RequiredRegDirectives{uc $regparts[2]}[4] = 1;
                            
                            # Check that root matches
                            AddError($Current_INF_File->getIndex($RegSection, $regcount), 2006, $RequiredRegDirectives{uc $regparts[2]}[0], $regparts[2]) 
                                    unless ($RequiredRegDirectives{uc $regparts[2]}[0] eq $regparts[0]);

                            # Check that the reg flags match
                            my $FlagsRequired = $RequiredRegDirectives{uc $regparts[2]}[2];
                            $FlagsRequired = hex($FlagsRequired) if ($FlagsRequired =~ /^0x[0-9A-F]*$/i);
                            my $FlagsInf = $regparts[3];
                            $FlagsInf =~ s/^\s*//i; # Kill leading spaces
                            $FlagsInf =~ s/\s*$//i; # Kill trailing spaces                                                        
                            $FlagsInf = hex($FlagsInf) if ($FlagsInf =~ /^0x[0-9A-F]*$/i);

                            AddError($Current_INF_File->getIndex($RegSection, $regcount), 2008, $RequiredRegDirectives{uc $regparts[2]}[2], $regparts[2])
                                    unless (($FlagsRequired & $FlagsInf) == $FlagsInf);                                    

                            # Check that the expected value is present                            
                            # The type for the CoInstaller32 reg-key is MULTI_SZ so multiple values
                            # may be specified comma-separated
                            my $valuefound = 0;
                            my $expectedvalue = $RequiredRegDirectives{uc $regparts[2]}[3];

                            my $valindex = 4; # 4 is the index that the values start at
                            while ($valindex < scalar @regparts) {
                                my $actualvalue = $regparts[$valindex];
                                $actualvalue =~  s/^\s*\"(.*)\"\s*/$1/;   # Strip the Quotes!
                                $actualvalue = uc $actualvalue unless ($actualvalue =~ /^0x[0-9A-F]*$/i);
                                $actualvalue = hex($actualvalue) if ($actualvalue =~ /^0x[0-9A-F]*$/i);
                            
                                if ($actualvalue =~ /$RequiredRegDirectives{uc $regparts[2]}[3]/i) {
                                    $valuefound = 1;
                                    last;
                                }
                                
                                $valindex++;
                            }                
                           
                            AddError($Current_INF_File->getIndex($RegSection, $regcount), 2016, "WUDFCoInstaller.dll' or 'WudfUpdate_NNNNN.dll") 
                                        unless ($valuefound == 1);
                        }                            
                                            
                        $regcount++;
                    }                    
                }
                # elsif chkinf.pm will catch the scenario where a section does not exist
            }
        }
        
        $count++;
    }

   
    # Check that all required directives were present
    my $RequiredDirective;
    foreach $RequiredDirective (keys(%RequiredRegDirectives)) {
        AddError($Current_INF_File->getIndex($Section, 0), 2016, $RequiredRegDirectives{$RequiredDirective}[3]) 
                unless ($RequiredRegDirectives{$RequiredDirective}[4] == 1);
    }
}

#-------------------------------------------------------------------------------------------------#
#-- CheckUmdfService($ServiceName) ---------------------------------------------------------------------------#
sub CheckUmdfService {
    my $Section = shift;
    my $count = 0;
    my $line;
    
    my($Directive, @Values, $Value, $Temp);
    
    my @lines = $Current_INF_File->getSection($Section);

    my (%RequiredDirectives) = ('UMDFLIBRARYVERSION'    => 0,
                                'SERVICEBINARY'         => 0,
                                'DRIVERCLSID'           => 0
                                );
                                
    foreach $line (@lines) {
        $count++, next if ($line =~ /^(\s*|;.*)$/);# Skip if it's a blank line
        
        ($Directive,$Value) = ChkInf::SplitLine($Section,$count,$line);
        
        if (defined($RequiredDirectives{uc $Directive})) {
            $RequiredDirectives{uc $Directive} = 1;  # Mark as found
        }
        
        if (uc $Directive eq 'UMDFLIBRARYVERSION') {
            my @VersionValues = split /\./, $Value;
            
            AddError($Current_INF_File->getIndex($Section,$count), 2012, $Directive, "1.0.0")
                unless (scalar @VersionValues == 3);
        }
        elsif (uc $Directive eq 'SERVICEBINARY') {
            # Check if the service binary is in %12%\UMDF, if not issue a warning
            $Value    =~  s/^\"(.*)\"/$1/;   # Strip the Quotes!
            
            AddWarning($Current_INF_File->getIndex($Section,$count), 3002)
                unless ($Value =~ /^\$WinDir\$\\system32\\drivers\\umdf\\.+$/i);
        }
        elsif (uc $Directive eq 'DRIVERCLSID') {   
            $Value    =~  s/^\s*\"(.*)\"\s*/$1/;   # Strip the Quotes!
            
            # braces around the guid are optional, so add them if not present
            if ($Value !~ /\A\{/ && $Value !~ /\}\z/) {
                $Value = "{" . $Value . "}";
            }
                
            AddError($Current_INF_File->getIndex($Section,$count), 2015)        
                        if (($Value !~ /^\{[a-fA-F0-9]{8}-[a-fA-F0-9]{4}-[a-fA-F0-9]{4}-[a-fA-F0-9]{4}-[a-fA-F0-9]{12}\}$/));
        }
        
        $count++;
    }    
    
    # Check that all required directives were present
    my $RequiredDirective;
    foreach $RequiredDirective (keys(%RequiredDirectives)) {
        AddError($Current_INF_File->getIndex($Section, 0), 2010, $RequiredDirective) 
                unless ($RequiredDirectives{$RequiredDirective} == 1);
    }
}

#-------------------------------------------------------------------------------------------------#
#-- CheckWUDFRdService($Model) ---------------------------------------------------------------------------#
sub CheckWUDFRdService {
    my $line;
    my $count = 0;
    my $Model    = $_;
       $Model    = uc($Model);
    my $Section = $Model.".Services";

    my($Directive, @Values, $Value, $Temp);

    # INF must have a [DDInstall.Services] for each [DDInstall] section
    unless ($Current_INF_File->sectionDefined($Section)) {   
        AddError(1, 2002, $Section);
        return;
    }

    # Check if the system provided sections are included
    # If they are, then we skip checking any WDF specific entries
    my $IncludeValue = "wpdmtp.inf";
    my $NeedsValue = "WPD.MTP.Services";
    
    if (is_defined($Section, "INCLUDE", $IncludeValue) &&
        is_defined($Section, "NEEDS", $NeedsValue)) 
    {   
        # Since the required system INF section is included we can break
        # out of the loop and not require checking the other entries
        return;
    }                            


    my @lines = $Current_INF_File->getSection($Section);
    my $hasSvcWUDFRD = 0;
    my %warnings = {};

    foreach $line (@lines) {
        $count++, next if ($line =~ /^(\s*|;.*)$/);# Skip if it's a blank line
        
        ($Directive,$Value) = ChkInf::SplitLine($Section,$count,$line);
        
        if (uc $Directive eq 'ADDSERVICE') {
            @Values = ChkInf::GetList($Value, $Current_INF_File->getIndex($Section, $count));
            
            if (scalar @Values == 3)
            {            
                my $svcSection = $Values[2];
                if ($Current_INF_File->sectionDefined($svcSection)) {
                    my @svclines = $Current_INF_File->getSection($svcSection);
                    my $svccount = 0;
                    my $svcline;
                    my $svcUsesWUDFRD = 0;

                    foreach $svcline (@svclines) {
                        $svccount++, next if ($svcline =~ /^(\s*|;.*)$/);# Skip if it's a blank line
                        
                        ($Directive,$Value) = ChkInf::SplitLine($svcSection,$svccount,$svcline);

                        if (uc $Directive eq 'SERVICEBINARY') {
                            # Check that WUDFrd.sys is being used as the service binary
                            $Value    =~  s/^\"(.*)\"/$1/;   # Strip the Quotes!
                            if ($Value =~ /^\$WinDir\$\\system32\\drivers\\wudfrd.sys$/i) {
                                $svcUsesWUDFRD  = 1;
                                $hasSvcWUDFRD = 1;
                            }
                            else {
                                $warnings{$svcSection} = $svccount;
                            }
                        }

                        $svccount++;
                    }

                    # If WUDFRd.sys was the service, validate that the settings are correct    
                    if ($svcUsesWUDFRD == 1)
                    {
                        my $expectedServiceType = 1;
                        my $expectedStartType = 3;
                        my $expectedErrorControl = 1;
                        
                        $svccount = 0;
                        foreach $svcline (@svclines) {
                            $svccount++, next if ($svcline =~ /^(\s*|;.*)$/);# Skip if it's a blank line
                            
                            ($Directive,$Value) = ChkInf::SplitLine($svcSection,$svccount,$svcline);

                            if (uc $Directive eq 'SERVICETYPE') {
                                AddWarning($Current_INF_File->getIndex($svcSection,$svccount), 3006, $Directive, $expectedServiceType)
                                    unless ($Value == $expectedServiceType);
                            }
                            elsif (uc $Directive eq 'STARTTYPE') {
                                AddWarning($Current_INF_File->getIndex($svcSection,$svccount), 3006, $Directive, $expectedStartType)
                                    unless ($Value == $expectedStartType);
                            }
                            elsif (uc $Directive eq 'ERRORCONTROL') {
                                AddWarning($Current_INF_File->getIndex($svcSection,$svccount), 3006, $Directive, $expectedErrorControl)
                                    unless ($Value == $expectedErrorControl);
                            }                        

                            $svccount++;
                        }
                    }                    
                } else {
                    # Specified section does not exist in the INF
                    # No need to AddError since chkinf.pm will flag this error
                }
            }
            else {
                # AddService directive is malformed
                # No need to AddError since chkinf.pm will flag this error
            }
        }
        
        $count++;
    }

    if ($hasSvcWUDFRD == 0) {
        for my $section(keys %warnings) {
            AddWarning($Current_INF_File->getIndex($section,$warnings{$section}), 3005);
        }
    }    
}


#-------------------------------------------------------------------------------------------------#
#-- CheckWDF($Model) ---------------------------------------------------------------------------#
sub CheckWDF {
    my $line;
    my $count = 0;
    my $Model    = $_;
       $Model    = uc($Model);
    my $Section = $Model.".Wdf";

    my($Directive, @Values, $Value, $Temp);

    # INF must have a [DDInstall.Wdf] for each [DDInstall] section
    unless ($Current_INF_File->sectionDefined($Section)) {   
        AddError(1, 2002, $Section);
        return;
    }

    # Check if the system provided sections are included
    # If they are, then we skip checking any WDF specific entries
    my $IncludeValue = "wpdmtp.inf";
    my $NeedsValue = "WPD.MTP.Wdf";
    
    if (is_defined($Section, "INCLUDE", $IncludeValue) &&
        is_defined($Section, "NEEDS", $NeedsValue)) 
    {   
        # Since the required system INF section is included we can break
        # out of the loop and not require checking the other entries
        return;
    }                            


    my @lines = $Current_INF_File->getSection($Section);

    my (%RequiredDirectives) = ('UMDFSERVICE'               => 0,
                                'UMDFSERVICEORDER'          => 0
                                );

    my @UmdfServiceOrderValues;
    my %UmdfServiceNames;
    my $UmdfServiceOrderLine = 0;
                                
    foreach $line (@lines) {
        $count++, next if ($line =~ /^(\s*|;.*)$/);# Skip if it's a blank line
        
        ($Directive,$Value) = ChkInf::SplitLine($Section,$count,$line);
        
        if (defined($RequiredDirectives{uc $Directive})) {
            $RequiredDirectives{uc $Directive} = 1;  # Mark as found
        }
        
        if (uc $Directive eq 'UMDFSERVICE') {
            @Values = ChkInf::GetList($Value, $Current_INF_File->getIndex($Section, $count));
            
            if (scalar @Values == 2)
            {            
                $UmdfServiceNames{uc $Values[0]} = 0;
            
                if ($Current_INF_File->sectionDefined($Values[1])) {
                    CheckUmdfService($Values[1]);
                } else {
                    AddError($Current_INF_File->getIndex($Section,$count), 2002, $Values[1]);
                }
            }
            else {
                AddError($Current_INF_File->getIndex($Section,$count), 2012, $Directive, "ServiceName, ServiceInstallSectionName");
            }
        }
        elsif (uc $Directive eq 'UMDFSERVICEORDER') {
            # Check if this has been already defined
            if ($UmdfServiceOrderLine != 0) {
                # Only one UmdfServiceOrder directive must be present
                AddError($Current_INF_File->getIndex($Section, $count), 2017, $Directive);
            }
            else {            
                # Post-process this once we have all service name values
                @UmdfServiceOrderValues = ChkInf::GetList($Value, $Current_INF_File->getIndex($Section, $count));
                $UmdfServiceOrderLine = $Current_INF_File->getIndex($Section, $count);
            }
        }
        elsif (uc $Directive eq 'UMDFIMPERSONATIONLEVEL') {
            my %UmdfImpersonationLevelValues = ('ANONYMOUS' => 0,
                                                'IDENTIFICATION' => 0,
                                                'IMPERSONATION' => 0,
                                                'DELEGATION' => 0
                                                );
                                                
            AddError($Current_INF_File->getIndex($Section, $count), 2011, $Directive) 
                unless (defined($UmdfImpersonationLevelValues{uc $Value}));
        }
        
        $count++;
    }          
    
    # Check that the service order contains only the service names
    # listed in UmdfService entries
    my $UmdfServiceName;
    foreach $UmdfServiceName (@UmdfServiceOrderValues) {
        if (defined($UmdfServiceNames{uc $UmdfServiceName})) {
            # Mark as found
            $UmdfServiceNames{uc $UmdfServiceName} = 1;
        }
        else {
            # ServiceOrder uses service name not defined using UmdfService directive
            AddError($UmdfServiceOrderLine, 2013, $UmdfServiceName);
        }
    }

    if ($UmdfServiceOrderLine != 0) {
        # Make sure that all service names are present in the ServiceOrder directive
        foreach $UmdfServiceName (keys(%UmdfServiceNames)) {
            AddError($UmdfServiceOrderLine, 2014, $UmdfServiceName)
                unless ($UmdfServiceNames{$UmdfServiceName} == 1);
        }
    }
    
    # Check that all required directives were present
    my $RequiredDirective;
    foreach $RequiredDirective (keys(%RequiredDirectives)) {
        AddError($Current_INF_File->getIndex($Section, 0), 2010, $RequiredDirective) 
                unless ($RequiredDirectives{$RequiredDirective} == 1);
    }     
}

#-------------------------------------------------------------------------------------------------#
# is_defined - Returns an array of refences to the parsed lines if a matching directive           #
#   with an (optionally) matching value is found in the specified section                         #
#-- is_defined($SectionName, $DirectivePattern, $ValuePattern) -----------------------------------#
sub is_defined
{
    my ($SectionName, $DirectivePattern, $ValuePattern) = @_;
    
    my @Lines; 
    my @SectionLines = $Current_INF_File->getSection($SectionName);
    my $count = 0;
    my ($Directive,$Value);

    foreach my $Line (@SectionLines)
    {
        ($Directive,$Value) = ChkInf::SplitLine($SectionName,$count,$Line);    
        
        if ($Directive =~ /^$DirectivePattern$/i)
        {
            if (!defined($ValuePattern) || $Value =~ /$ValuePattern/i )
            {
                push(@Lines, $Line);
            }
        }
        
        $count++;
    }

    return @Lines;
}

#-------------------------------------------------------------------------------------------------#
# CheckCopyFiles - Checks the CopyFiles directive of the INF to make sure that system supplied    #
#   files are not specified for copying directly by the INF. The INF must instead use the Include #
#   and Needs directive to copy the appropriate system files
#-- CheckCopyFiles($Model) -----------------------------------------------------------------------#
sub CheckCopyFiles{
    my $line;
    my $count = 0;
    my $Model    = $_;
       $Model    = uc($Model);
    my $Section = $Model;

    my($Directive, @Values, $Value, $Temp);

    my @lines = $Current_INF_File->getSection($Section);

    # BannedCopyFiles is a map of the file to check with the Include directive as the first element of the value array
    # and the Needs directive as the second element of the value array
    # e.g. 'myfile.dll' => ["Include.inf", "Need.Section"]
    my (%BannedCopyFiles) = ('WPDMTPDR.DLL'            => ["wpdmtp.inf", "WPD.MTP"],
                             'WPDMTP.DLL'              => ["wpdmtp.inf", "WPD.MTP"],
                             'WPDMTPUS.DLL'            => ["wpdmtp.inf", "WPD.MTP"],
                             'WPDCONNS.DLL'            => ["wpdmtp.inf", "WPD.MTP"],
                             'WPDUSB.SYS'              => ["wpdmtp.inf", "WPD.MTP"],
                             );

    # BannedServices is a list of binaries that if specified for copying must be set up
    # for service registration by Include'ing the system supplied INF service section
    my (%BannedServices) = ('WPDUSB.SYS'            => ["wpdmtp.inf", "WPD.MTP.Services"],
                            );

    foreach $line (@lines) {
        $count++, next if ($line =~ /^(\s*|;.*)$/);# Skip if it's a blank line
        
        ($Directive,$Value) = ChkInf::SplitLine($Section,$count,$line);

        if (uc $Directive eq "COPYFILES") {
            my @CopySections = split /,/,$Value;
            my $CopySection;

            foreach $CopySection (@CopySections) {
                $CopySection =~ s/^\s*//i; # Kill leading spaces
                $CopySection =~ s/\s*$//i; # Kill trailing spaces
                
                if ( $Current_INF_File->sectionDefined($CopySection) ) {
                    my @CopySectionLines = $Current_INF_File->getSection($CopySection);
                    my $cfcount = 0;
                    my $cfline;
                    
                    foreach $cfline (@CopySectionLines) {
                        $cfcount++, next if ($cfline =~ /^(\s*|;.*)$/);# Skip if it's a blank line

                        $cfline =~ s/^\s*//i; # Kill leading spaces
                        $cfline =~ s/\s*$//i; # Kill trailing spaces
                                        
                        if (defined($BannedCopyFiles{uc $cfline})) {
                            if (!is_defined($Model, "INCLUDE", $BannedCopyFiles{uc $cfline}[0]) ||
                                !is_defined($Model, "NEEDS", $BannedCopyFiles{uc $cfline}[1])) 
                            {                                
                                AddWarning($Current_INF_File->getIndex($CopySection,$cfcount), 3003, $cfline, 
                                        "INCLUDE=".$BannedCopyFiles{uc $cfline}[0]." & NEEDS=".$BannedCopyFiles{uc $cfline}[1], "DDInstall");
                            }                            
                        }

                        if (defined($BannedServices{uc $cfline})) {
                            if (!is_defined($Model.".Services", "INCLUDE", $BannedServices{uc $cfline}[0]) ||
                                !is_defined($Model.".Services", "NEEDS", $BannedServices{uc $cfline}[1])) 
                            {                                
                                AddWarning($Current_INF_File->getIndex($CopySection,$cfcount), 3003, $cfline, 
                                        "INCLUDE=".$BannedServices{uc $cfline}[0]." & NEEDS=".$BannedServices{uc $cfline}[1], "DDInstall.Services");
                            }                            
                        }                            
                                            
                        $cfcount++;
                    }                    
                }
                # elsif chkinf.pm will catch the scenario where a section does not exist
            }
        }
        
        $count++;
    }           
}

#-------------------------------------------------------------------------------------------------#
#-- CheckAddReg($Model) ---------------------------------------------------------------------------#
sub CheckAddReg {
    my $line;
    my $count = 0;
    my $Model    = $_;
       $Model    = uc($Model);
    my $Section = $Model.".hw";

    my($Directive, @Values, $Value, $Temp);

    my @lines = $Current_INF_File->getSection($Section);

    my (%OptionalRegDirectives) = ('ENABLELEGACYSUPPORT'            => ["HKR", "", "0x10001", ["0x0", "0x1", "0x2", "0x3"] ],
                                   'ENABLEDEFAULTAUTOPLAYSUPPORT'   => ["HKR", "", "0x10001", ["0x0", "0x1"] ],
                                   'USEWIAAUTOPLAY'                 => ["HKR", "", "0x10001", ["0x0", "0x1"] ]
                                   );

    foreach $line (@lines) {
        $count++, next if ($line =~ /^(\s*|;.*)$/);# Skip if it's a blank line
        
        ($Directive,$Value) = ChkInf::SplitLine($Section,$count,$line);

        if (uc $Directive eq "ADDREG") {
            my @RegSections = split /,/,$Value;
            my $RegSection;
            
            foreach $RegSection (@RegSections) {
                $RegSection =~ s/^\s*//i; # Kill leading spaces
                $RegSection =~ s/\s*$//i; # Kill trailing spaces
                
                if ( $Current_INF_File->sectionDefined($RegSection) ) {
                    my @RegSectionLines = $Current_INF_File->getSection($RegSection);
                    my $regcount = 0;
                    my $regline;
                    
                    foreach $regline (@RegSectionLines) {
                        $regcount++, next if ($regline =~ /^(\s*|;.*)$/);# Skip if it's a blank line
                        
                        my @regparts = split /,/,$regline;
                        
                        $regcount++, next if ($regparts[2] =~ /^(\s*|;.*)$/);# Skip if it's a blank entry
                        
                        $regparts[2]    =~  s/^\"(.*)\"/$1/;   # And Strip the Quotes!
                
                        if (defined($OptionalRegDirectives{uc $regparts[2]})) {
                            
                            # Check that root matches
                            AddError($Current_INF_File->getIndex($RegSection, $regcount), 2006, $OptionalRegDirectives{uc $regparts[2]}[0], $regparts[2]) 
                                    unless ($OptionalRegDirectives{uc $regparts[2]}[0] eq $regparts[0]);

                            # Check that the reg flags match
                            my $FlagsRequired = $OptionalRegDirectives{uc $regparts[2]}[2];
                            $FlagsRequired = hex($FlagsRequired) if ($FlagsRequired =~ /^0x[0-9A-F]*$/i);
                            my $FlagsInf = $regparts[3];
                            $FlagsInf =~ s/^\s*//i; # Kill leading spaces
                            $FlagsInf =~ s/\s*$//i; # Kill trailing spaces                                                        
                            $FlagsInf = hex($FlagsInf) if ($FlagsInf =~ /^0x[0-9A-F]*$/i);

                            AddError($Current_INF_File->getIndex($RegSection, $regcount), 2008, $OptionalRegDirectives{uc $regparts[2]}[2], $regparts[2])
                                    unless (($FlagsRequired & $FlagsInf) == $FlagsInf);
                                    
                            # Check that the value matches at least one of the expected values
                            my $actualvalue = $regparts[4];
                            $actualvalue = uc $actualvalue unless ($actualvalue =~ /^0x[0-9A-F]*$/i);
                            $actualvalue = hex($actualvalue) if ($actualvalue =~ /^0x[0-9A-F]*$/i);                                
                            
                            my $valuefound = 0;
                            my $expectedvalue;
                            
                            foreach $expectedvalue (@{$OptionalRegDirectives{uc $regparts[2]}[3]}) {
                                $expectedvalue = hex($expectedvalue) if ($expectedvalue =~ /^0x[0-9A-F]*$/i);
                                
                                if ($expectedvalue eq $actualvalue)
                                {
                                    $valuefound = 1;
                                    last;
                                }
                            }
                            
                            AddError($Current_INF_File->getIndex($RegSection, $regcount), 2007, $regparts[2]) unless ($valuefound==1);
                        }
                            
                                            
                        $regcount++;
                    }                    
                }
                # elsif chkinf.pm will catch the scenario where a section does not exist
            }
        }
        
        $count++;
    }           
}

#-------------------------------------------------------------------------------------------------#
#-- CheckModel($Model) ---------------------------------------------------------------------------#
sub CheckModel {

    my $line;
    my $Section  =  shift;
    my @lines    =  $Current_INF_File->getSection($Section);
    my $count    =  0;
    my $Model    = $_;
       $Model    = uc($Model);

    my($Directive, @Values, $Value, $Temp);

    print(STDERR "\tEntering CheckModel($Model)...\n") if ($DEBUG);

    # Check WPD specific registry entries
    CheckAddReg($_);   

    # Check that files from inbox INFs are not being copied directly
    CheckCopyFiles($_); 
    
    # Check WDF CoInstaller entries
    CheckWDFCoInstaller($_);
    
    # Check WDF entries
    CheckWDF($_);

    # Check if WUDFRd.sys is specified as the service
    CheckWUDFRdService($_);
}

#---------------------------------------------------#
# Routine for checking device class specific
# directives that occur in common sections verified
# by ChkINF.  Return 0 if directive is unknown or
# 1 if the directive is known (you should also verify
# it and write any appropriate warnings/errors.
#---------------------------------------------------#
sub DCDirective 
{
    my ($Module, $Directive) = @_;
    
    # WPD specific directives
    my %ValidDirectives = 
    ( 
    );

    # if defined, return true, else false    
    return defined($ValidDirectives{uc $Directive}); 
}

#---------------------------------------------------#
# Verifies that the module loaded correctly.  Be sure
# to change the string to the name of your module.
#---------------------------------------------------#
sub Exists {
    print(STDERR "Module \"WPD\" Loaded\n");
    return(1); # return 1 on success
}


#-------------------------------------------------------------------------------------------------#
#-- ExpandModelSection($line, $Section) ----------------------------------------------------------#
sub ExpandModelSection {
    my $Section  = shift;
    my $line;
    my @lines    = $Current_INF_File->getSection($Section);
    my $count    = 0;

    my(%Models)  =  ("","");
    my($TempVar);

    my($Directive, @Values, $Value);

    print(STDERR "\tInvoking Display::ExpandModelSection...\n") if ($DEBUG);

    foreach $line (@lines) {
        $count++, next if ($line =~ /^(\s*|;.*)$/);# Skip if it's a blank line
        
        ($Directive,$Value) = ChkInf::SplitLine($Section,$count,$line);
        @Values             = ChkInf::GetList($Value,$Current_INF_File->getIndex($Section,$count));

        $Values[0] =~ s/^\s*//i; # Kill leading spaces
        $Values[0] =~ s/\s*$//i; # Kill trailing spaces

        if (defined($Values[0])) {
            $Values[1] = "x" unless ( defined($Values[1]) );

            # Check if there are any Win2k or later entries as well
            my $DecoratedSectionFound = 0;
            my $Decoration;
            my(@Decorations) = (".NT", ".NTx86", ".NTamd64", ".NTia64");
            foreach $Decoration (@Decorations) {       
                my $DecoratedSection;
                $DecoratedSection = $Values[0].$Decoration;
                     
                if ( $Current_INF_File->sectionDefined($DecoratedSection) ) {
                    $Models{$DecoratedSection} = "";
                    $DecoratedSectionFound = 1;
                }
            }

            # Check the undecorated section only if the NT sections are not present        
            if (!$DecoratedSectionFound && $Current_INF_File->sectionDefined($Values[0]))
            {
                $Models{$Values[0]} = $Values[1];
            }            
        }
                
        $count++;
    }
    return(%Models);
}

#---------------------------------------------------#
# The module's main checking routine.
# This is your entry point for verifying an INF
#---------------------------------------------------#
sub InfCheck {

    print("\tBeginning WPD::InfCheck(@_)\n") if ($DEBUG);

    unless ( $Current_INF_File->sectionDefined("Manufacturer") ) {
        print(STDERR "\t$Current_INF_File->{inffile} contains no [Manufacturer] section.\n") if ($DEBUG);
        return(FALSE)
    }

    %AllModels  = LearnModels();

    # For every model, check the DDInstall section
    foreach (sort(keys(%AllModels))) {
        warn("\tUndefined model found in array: $_\n"), next if (!defined($AllModels{$_}));
        
        if ( $Current_INF_File->sectionDefined($_) ) {        
            CheckModel($_) unless ($AllModels{$_} eq "DONE");
            $AllModels{$_} = "DONE";
        }
    }
        
    return(1); # return 1 on success
}


#---------------------------------------------------#
# Is called when module is first included.
# Use it to set your per-inf variables.
#---------------------------------------------------#
sub InitGlobals {
    # First parameter is the name of the current INF.
    $Current_INF_File = $_[1];

    # Output file = htm subdir + ((INF Name - .inf) + .htm)
    $Current_HTM_File = $Current_INF_File->{inffile};
    $Current_HTM_File =~ s/\.inf$/\.htm/i;
    $Current_HTM_File = "htm\\".substr($Current_HTM_File,rindex($Current_HTM_File, "\\")+1);

    # Second parameter is a list of Device Specific options.
    my($DC_Specific_Options) = $_[2];

    # $DC_Specific_Options is a string that contains all device
    #   class options delimited by &'s
    if (defined($DC_Specific_Options)) {
        @Device_Class_Options = split(/&/,$DC_Specific_Options);
    } else {
        @Device_Class_Options = ("NULL","NULL");
    }

    # Array must be shifted since first element also had a & prepended
    shift(@Device_Class_Options);

    return(1); # return 1 on success
}


#-------------------------------------------------------------------------------------------------#
# LearnModels - Returns an array of all models listed in the INF                                  #
#-- LearnModels() --------------------------------------------------------------------------------#
sub LearnModels {
    return(FALSE) if (! $Current_INF_File->sectionDefined("Manufacturer") );

    my $Section = "Manufacturer";
    my $line;
    my @lines   = $Current_INF_File->getSection("Manufacturer");
    my $count   = 0;

    my($Directive, $Value);
    my %Models;

    print(STDERR "\tInvoking Display::ProcManu...\n") if ($DEBUG);

    foreach $line (@lines) {
        $count++, next if ($line =~ /^\s*$/); # Skip if it's a blank line

        ($Directive,$Value) = ChkInf::SplitLine($Section,$count,$line);

        # [Manufacturers] may not have both a directive & a value
        if ($Value =~ /^\s*$/) {
            $Value     = $Directive;
            $Directive = "";
        } else {
            $Value    =~  s/^\"(.*)\"/$1/;   # And Strip the Quotes!
        }

        my @Values;
        my $NumValues;
        @Values = split /,/,$Value;
        $NumValues = scalar @Values;
        
        if ($NumValues == 1 || $NumValues == 0) {
            # No TargetOsVersion has been defined
            $Value =~ s/^\s*//; # Kill leading spaces
            $Value =~ s/\s*$//; # Kill trailing spaces

            if ( $Current_INF_File->sectionDefined($Value) ) {
                # Add the models from $INFSections{$Value} to our running list
                %Models = (%Models, ExpandModelSection($Value));
            }
        }
        else {
            # One or more TargetOsVersions have been defined
            # Check if the undecorated model section name exists
            my $ModelSectionName;
            $ModelSectionName = shift(@Values);
            
            $ModelSectionName =~ s/^\s*//; # Kill leading spaces
            $ModelSectionName =~ s/\s*$//; # Kill trailing spaces

            if ( $Current_INF_File->sectionDefined($ModelSectionName) ) {
                # Add the models from $INFSections{$ModelSectionName} to our running list
                %Models = (%Models, ExpandModelSection($ModelSectionName));
            }
            
            # Check if decorated model section names exist
            foreach $Value (@Values) {
                $Value =~ s/^\s*//; # Kill leading spaces
                $Value =~ s/\s*$//; # Kill trailing spaces
                
                # Add this to the model section name
                $Value = $ModelSectionName.".".$Value;
                                
                if ( $Current_INF_File->sectionDefined($Value) ) {
                    # Add the models from $INFSections{$Value} to our running list
                    %Models = (%Models, ExpandModelSection($Value));
                }
            }            
        }
        
        $count++;
    }
    
    return(%Models);
}

#---------------------------------------------------#
# Allows to add Device specific information to the
# top of the INF summary page.  The information here
# should be brief and link to detailed summaries
# below. (Which will be written using PrintDetials).
#---------------------------------------------------#
sub PrintHeader {
    return(1); # return 1 on success
}


#---------------------------------------------------#
# Allows addition of device specific results to the
# detail section on the INF summary.
#---------------------------------------------------#
sub PrintDetails {
    return(1); # return 1 on success
}

###################################################################################################
#+-----------------------------------------------------------------------------------------------+#
#|                                   HELPER SUBROUTINES                                          |#
#+-----------------------------------------------------------------------------------------------+#
###################################################################################################

#-------------------------------------------------------------------------------------------------#
# This sub adds a new error to the list of errors for the file.                                   #
#-- AddError($ErrorNum, $LineNum, @ErrorArgs) ----------------------------------------------------#
sub AddError {
    my($LineNum) = $_[0], shift;
    my($ErrorNum)  = $_[0], shift;
    my(@ErrorArgs)= @_;

    $ErrorArgs[0] = " " if (! defined($ErrorArgs[0]));
    my($str, $this_str, $info_err, $format_err);

    $info_err = $ErrorTable{"$ErrorNum"};

    if ( !defined($info_err) ) {
        $this_str = "Unknown error $ErrorNum.";
    } else {
        $format_err = $$info_err[0];
        $this_str = sprintf($format_err, @ErrorArgs);
    }
    
    ChkInf::AddDeviceSpecificError($LineNum, $ErrorNum, $this_str);
}

#-------------------------------------------------------------------------------------------------#
# This sub adds a new warning to the list of warnings for the file.                               #
#-- AddWarning($WarnNum, $LineNum, @ErrorArgs) ---------------------------------------------------#
sub AddWarning {
    my($LineNum)  = $_[0], shift;
    my($WarnNum)  = $_[0], shift;
    my(@WarnArgs) = @_;

    $WarnArgs[0] = " " if (! defined($WarnArgs[0]));
    my($str, $this_str, $info_wrn, $format_wrn);

    $info_wrn = $WarningTable{"$WarnNum"};

    if ( !defined($info_wrn) ) {
        $this_str = "Unknown warning $WarnNum.";
    } else {
        $format_wrn = $$info_wrn[0];
        $this_str = sprintf($format_wrn, @WarnArgs);
    }
    ChkInf::AddDeviceSpecificWarning($LineNum, $WarnNum, $this_str);
}

#-------------------------------------------------------------------------------------------------#
# Verifies DEVICEDATA Sections                                                                    #
#-- VerifyDeviceData($linenum, $SecName) ---------------------------------------------------------#
sub VerifyDeviceData {
    my $line;
    my $Section =  shift;
    my @lines   = $Current_INF_File->getSection($Section);
    my $count    = 0;

    my($Directive, $Value);

    print(STDERR "\tBeginning VerifyDeviceData...\n") if ($DEBUG);
    foreach $line (@lines) {
        $count++, next if ($line =~ /^\s*$/); # Skip if it's a blank line

        ($Directive,$Value) = ChkInf::SplitLine($Section,$count,$line);
        $count++;
    }

    ChkInf::Mark($Section);

    return(1);
}

#-------------------------------------------------------------------------------------------------#
# Verifies EVENTS Sections                                                                        #
#-- VerifyEvents($linenum, $SecName) -------------------------------------------------------------#
sub VerifyEvents {
    my $line;
    my $Section  =  shift;
    my @lines    = $Current_INF_File->getSection($Section);
    my $count    = 0;

    my($Directive, @Values, $Value);

    print(STDERR "\tBeginning VerifyEvents...\n") if ($DEBUG);
    foreach $line (@lines) {
        $count++, next if ($line =~ /^\s*$/); # Skip if it's a blank line

        ($Directive,$Value) = ChkInf::SplitLine($Section,$count,$line);
        @Values = ChkInf::GetList($Value,$Current_INF_File->getIndex($Section, $count));

        AddError($Current_INF_File->getIndex($Section, $count), 2003, "Directive", "have 3 parameters") unless (defined($Values[2]) );
        $count++;
    }

    ChkInf::Mark($Section);

    return(1);
}

#-------------------------------------------------------------------------------------------------#
# Verifies UNINSTALLSECTION Sections                                                              #
#-- VerifyUninstallSectionEvents($linenum, $SecName) ---------------------------------------------#
sub VerifyUninstallSection {
    my $line;
    my($Section) =  shift;
    my @lines    = $Current_INF_File->getSection($Section);
    my $count    = 0;

    my($Directive, $Value);

    my(%Directives) =  ("DELFILES" => TRUE,
                        "DELREG"   => TRUE);

    foreach $line (@lines) {
        $count++, next if ($line =~ /^\s*$/); # Skip if it's a blank line

        ($Directive,$Value) = ChkInf::SplitLine($Section,$count,$line);

        if (defined $Directives{uc($Directive)}) {
            $Directive = uc($Directive);
            ChkInf::DELFILES($Section,$count,$Value) if ($Directive eq "DELFILES");
            ChkInf::DELREG($Section,$count,$Value)   if ($Directive eq "DELREG");
        } else {
            AddError($Current_INF_File->getIndex($Section,$count), 2005, $Directive);
        }

        $count++;
    }

    ChkInf::Mark($Section);

    return(1);
}

return(1); # Module must return 1 to let chkinf know it loaded correctly.
