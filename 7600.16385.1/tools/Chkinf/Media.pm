# (C) Copyright 1999-2003 Microsoft Corporation

# The name of this module. For our purposes it must be all in caps.
package MEDIA;

    use strict;                # Keep the code clean
    #---------------------------------------------------#
    # Module level variables
    #---------------------------------------------------#
    my $Version  = "5.1.2402.0"; # Version of this file
    my $inffile;                 # Stores name of the current INF file.
    my $DEBUG    = 0;            # True for debug spew
    my %SectionsChecked;         # Hash for every section looked at
    #
    # Required, but currently unused routines
    #
    sub PrintHeader  { return(1); }
    sub PrintDetails { return(1); }
    
return(1); # Module must return 1 to let chkinf know it loaded correctly.
#---------------------------------------------------#
# Routine for checking device class specific
# directives that occur in common sections verified
# by ChkINF.  Return 0 if directive is unknown or
# 1 if the directive is known (you should also verify
# it and write any appropriate warnings/errors.
#---------------------------------------------------#
sub DCDirective {
    shift;
    my $Directive        = shift;
    my %ValidDirectives  = ( "KNOWNREGENTRIES" => 1,
                             "KNOWNFILES"      => 1);
    return(0) unless (defined $ValidDirectives{$Directive});
    return(1);
}

#---------------------------------------------------#
# Verifies that the module loaded correctly.  Be sure
# to change the string to the name of your module.
#---------------------------------------------------#
sub Exists {
    print(STDERR "Module \"MEDIA\" Loaded\n");
    return(1); # return 1 on success
}

#---------------------------------------------------#
# The module's main checking routine.
# This is your entry point for verifying an INF
#---------------------------------------------------#
sub InfCheck {
    my %AllDevices = LearnModels();
    #
    # AllDevices{DEVICE_ID}{PROVIDER}
    #                      {DESCRIPTION}
    #                      {DDINSTALL}
    #
    my $DeviceID;
    my $Section;
    my $SkipUndecorated = 0;
    
    print(STDERR "\tInvoking Media::InfCheck, looping through AllDevices...\n") if ($DEBUG);
    foreach $DeviceID (keys %AllDevices) {
        $Section      = $AllDevices{uc $DeviceID}{"DDINSTALL"};
        # Check all existing NT/2000 sections
        if ( $inffile->sectionDefined("$Section.NT") ) {
            ProcessDeviceID($DeviceID,"NT",$AllDevices{$DeviceID});
            $SkipUndecorated++;
        }
        if ( $inffile->sectionDefined("$Section.NTx86") ) {
            ProcessDeviceID($DeviceID,"NTx86",$AllDevices{$DeviceID});
            $SkipUndecorated++;
        }
        if ( $inffile->sectionDefined("$Section.NTamd64") ) {
            ProcessDeviceID($DeviceID,"NTamd64",$AllDevices{$DeviceID});
            $SkipUndecorated++;
        }
        if ( $inffile->sectionDefined("$Section.NTia64") ) {
            ProcessDeviceID($DeviceID,"NTia64",$AllDevices{$DeviceID});
            $SkipUndecorated++;
        }
        # Only check default if no NT/2000 section is defined
        if ( $inffile->sectionDefined("$Section") and (! $SkipUndecorated) ){
            ProcessDeviceID($DeviceID,"",$AllDevices{$DeviceID});
        }
    }

    return(1); # return 1 on success
}

#---------------------------------------------------#
# Is called when module is first included.
# Use it to set your per-inf variables.
#---------------------------------------------------#
sub InitGlobals {
    shift;
    # First parameter is the INF object
    $inffile=shift;
    
    # Media.pm does not accept DeviceClassOptions, so ignore
    # the rest of the parameters.

    # Reset globals
    %SectionsChecked=();
    return(1); # return 1 on success
}

###################################################################################################
#+-----------------------------------------------------------------------------------------------+#
#|                                 CLASS SPECIFIC SUBROUTINES                                    |#
#+-----------------------------------------------------------------------------------------------+#
###################################################################################################

#-------------------------------------------------------------------------------------------------#
# This sub adds a new error to the list of errors for the file.                                   #
#-- AddMediaError($ErrorNum, $LineNum, @ErrorArgs) -----------------------------------------------#
sub AddMediaError {

	my %MediaErrorTable = (
    	    # 8001 => ["GUID %s requires the non-localizable reference string \"%s\"."],
	    8002 => ["All audio port device install sections must contain 'NEEDS=%s'."],
	    8003 => ["All audio port device install sections must contain 'INCLUDE=%s'."],
            8004 => ["Invalid HD Audio device ID: %s. Device ID should not match to two part Hardware IDs."],
            8005 => ["Endpoint property registry root %s must be 'HKR'"],
            8006 => ["Endpoint property subkey %s not in the format of 'EP\\\\x'."],
            8007 => ["Endpoint property subkey %s breaks the sequence, should be EP\\\\%s."],
            8008 => ["Endpoint property subkey for OEM periodicity must have REG_BINARY(0x00000001) flag set instead of %s."],
            8009 => ["Endpoint property subkey for OEM periodicity must be of PROPVARIANT type VT_BLOB(0x0041) instead of 0x%x."],
            8010 => ["Endpoint property subkey for OEM periodicity must have serialized data size at 8 bytes instead of %s."],
            8011 => ["Endpoint property subkey for OEM periodicity must have data between 5-9 ms (50000-90000 hns) and round up to closest ms, instead of %s hns."],
            8012 => ["Endpoint property subkey for OEM periodicity must have value list of all hex numbers instead of %s."],
            8013 => ["Invalid Bluetooth Audio Device ID: %s. Device ID should match to specific Vendor ID and Product ID."],
            8014 => ["Invalid Vendor ID Type (%s) in Bluetooth Audio Device ID. Vendor ID Type can either be 0001 for Bluetooth SIG Vendor ID or 0002 for USB Vendor ID"],
    );

    my $ErrorNum  = shift;
    my $LineNum   = shift;
    my @ErrorArgs = @_;

    $ErrorArgs[0] = " " if (! defined($ErrorArgs[0]));
    my($str, $this_str, $info_err, $format_err);

    $info_err = $MediaErrorTable{$ErrorNum};

    if ( !defined($info_err) ) {
        $this_str = "Unknown error $ErrorNum.";
    } else {
        $format_err = $$info_err[0];
        $this_str = sprintf($format_err, @ErrorArgs);
    }
    ChkInf::AddDeviceSpecificError($LineNum, $ErrorNum, $this_str);
}

#-------------------------------------------------------------------------------------------------#
#-- ExpandModelSection($Section, $Provider) ------------------------------------------------------#
sub ExpandModelSection {
    my $Section  = shift;
    my $Provider = shift;
    my $line;
    my @lines    =  $inffile->getSection($Section);
    my $count    = 0;

    my %Models   =  ();
    my $TempVar;

    my ($DevDesc, @Values, $Value);
    my ($DevSection, $DevID);

    print(STDERR "\t\tInvoking Media::ExpandModelSection...\n") if ($DEBUG);

    foreach $line (@lines) {
        $count++, next if ($line =~ /^\s*$/);# Skip if it's a blank line

        ($DevDesc,$Value) = ChkInf::SplitLine($Section,$count,$line);
        @Values           = ChkInf::GetList($Value,$inffile->getIndex($Section,$count));

        print (STDERR "\t\t\tDevDesc: $DevDesc\n") if ($DEBUG);

        if (defined $Values[0]) {
            $DevSection = $Values[0];
            print(STDERR "\t\t\tDevSection: $DevSection\n") if ($DEBUG);
        } else {
            # ERROR
            next;
        }
    
        if (defined $Values[1]) {
            $DevID = $Values[1];
            print(STDERR "\t\t\tDevID: $DevID\n") if ($DEBUG);
        } else {
            # ERROR
            next;
        }

        if (defined($Values[0])) {
            if (defined $Models{uc $DevID}) {
                # ERROR
            } else {
               $Models{uc $DevID}{"DDINSTALL"}   = $DevSection;
               $Models{uc $DevID}{"DESCRIPTION"} = $DevDesc;
               $Models{uc $DevID}{"PROVIDER"}    = $Provider;
            }
        }
        $count++;
    }

    return(%Models);
}

#-------------------------------------------------------------------------------------------------#
# Interfaces - returns all addinterfaces (w/ string tokens removed) from a section.               #
#-- Interfaces() ---------------------------------------------------------------------------------#
sub Interfaces {
    my $line;
    my $Section  =  shift;
    my @lines    =  $inffile->getSection($Section);
    my $count    =  0;
    my @interfaces;
    my ($Directive,$Value);

    print(STDERR "\t\tInvoking Media::Interfaces...\n") if ($DEBUG);
    print(STDERR "\t\t\tSection: $Section\n") if ($DEBUG);

    foreach $line (@lines) {
        $count++, next if ($line =~ /^[\s\t]*$/);
        ($Directive,$Value) = ChkInf::SplitLine($Section,$count,$line);
        
        print(STDERR "\t\t\t[$count]Directive: $Directive\n") if ($DEBUG);
        print(STDERR "\t\t\t[$count]Value: $Value\n") if ($DEBUG);
        
        if (uc $Directive eq "ADDINTERFACE") {
            # Update our list with [$value, $linenum]
            push(@interfaces, [$Value,$inffile->getIndex($Section,$count)]);
        }

        $count++;
    }

    return(@interfaces);
}

#-------------------------------------------------------------------------------------------------#
# LearnModels - Returns an array of all models listed in the INF                                  #
#-- LearnModels() --------------------------------------------------------------------------------#
sub LearnModels {
    return() if (! $inffile->sectionDefined("Manufacturer") );
    my $Section = "Manufacturer";
    my $line;
    my @lines   = $inffile->getSection("Manufacturer");
    my $count   = 0;

    my($Provider, $DevSection);
    my(%Models);
    my($ModelSectionName);
    my(@TargetOSVersions);
    my($ModelSectionName);
    my($TargetOSVersion);
    my(@SectionProviderPairs);
    my($SectionProviderPair);

    print(STDERR "\tInvoking Media::LearnModels...\n") if ($DEBUG);

    foreach $line (@lines) {
        print(STDERR "\t\tline: $line\n") if ($DEBUG);

        $count++, next if ($line =~ /^\s*$/); # Skip if it's a blank line

        ($Provider,$DevSection) = ChkInf::SplitLine($Section,$count,$line);
        
        print(STDERR "\t\tProvider: $Provider\n") if ($DEBUG);
        # [Manufacturers] may not have both a directive & a value
        if ($DevSection =~ /^\s*$/) {                                   # DevSection is empty
            push @SectionProviderPairs, [($Provider, $Provider)];
        } else {
            $DevSection    =~  s/^\"(.*)\"/$1/;                         # And Strip the Quotes!
            print(STDERR "\t\tDevSection: $DevSection\n") if ($DEBUG);
        
	    # we split the string following the strkey
            @TargetOSVersions = split('\s*,\s*', $DevSection);          # we split the string based on comma but also remove all the whitespaces around the comma
            $ModelSectionName = shift(@TargetOSVersions);               # but the first element is models-section-name, remove the first element from the array of target OS versions
            
            print(STDERR "\t\tModelSectionName: $ModelSectionName\n") if ($DEBUG);
            
            if ($#TargetOSVersions eq -1) {                             # Target OS Versions is empty
                push @SectionProviderPairs, [($Provider, $ModelSectionName)];
            } else {
                foreach $TargetOSVersion (@TargetOSVersions) {
                    print(STDERR "\t\tTargetOSVersion: $TargetOSVersion\n") if ($DEBUG);
   
                    $DevSection = "$ModelSectionName.$TargetOSVersion";
                    print(STDERR "\t\t\tDevSection: $DevSection\n") if ($DEBUG);
                    push @SectionProviderPairs, [($Provider, $DevSection)];
                }
            }
        }
        $count++;
    }

    foreach $SectionProviderPair (@SectionProviderPairs) {
        ($Provider, $DevSection) = @{$SectionProviderPair};
        print(STDERR "\t\tExpanding Model for (Provider:$Provider)$DevSection\n");
        
        if ( $inffile->sectionDefined($DevSection) ) {
            # Add the models from $INFSections{$DevSection} to our running list
            %Models = (%Models, ExpandModelSection($DevSection, $Provider));
        }
    }

    return(%Models);
}

#-------------------------------------------------------------------------------------------------#
# ProcessDeviceID - Validate a Device Installation                                                #
#-- ProcessDeviceID() ----------------------------------------------------------------------------#
sub ProcessDeviceID {
    my $DeviceID     = shift;
    my $Extension    = shift;
    # Get the passed hash
    my %Device       = %{$_[0]}; shift;

    print(STDERR "\tInvoking Media::ProcessDeviceID...\n") if ($DEBUG);
    print(STDERR "\t\tDeviceID: $DeviceID\n") if ($DEBUG);
    
    my $Section      = $Device{"DDINSTALL"};
    if ($Extension ne "") {
       print(STDERR "\t\tExtension: $Extension\n") if ($DEBUG);
       $Section     = "$Section.$Extension";
    }
    
    print(STDERR "\t\tSection: $Section\n") if ($DEBUG);
    
    # Verify all DeviceID to make sure it's what we want
    # We need to do this before verifying that the section for the ID is checked because
    # multiple DeviceID can have the same section
    VerifyDeviceID($Section,$DeviceID);

    return if (defined $SectionsChecked{uc $Section});
    return if (! $inffile->sectionDefined($Section) );

    
    # Define so section isn't checked again.
    $SectionsChecked{uc $Section} = 1;

    my $line;
    my @lines   = $inffile->getSection($Section);
    
    my $cIfaceLns = 0;       # count of interface lines
    my @InterfaceLines;      # entries under [Section].Interface
    my $InterfaceLine;       # each entry in [Section].Interface    
    
    my $count   = 0;
    my @Include;
    my @Needs;
    my @RegLines;
    my $RegLine;
    my @Interfaces;
    my $Interface;

    my ($Directive,$Value);
    my ($GUID,$RefStr,$Sec,$Flags);
    my $IsAudio = 0;

    my %KnownGUIDS = (
        "6994AD04-93EF-11D0-A3CC-00A0C9223196" => "WAVE",      # AUDIO
        "65E8773E-8F56-11D0-A3B9-00A0C9223196" => "WAVE",      # RENDER
        "65E8773D-8F56-11D0-A3B9-00A0C9223196" => "WAVE",      # CAPTURE
        "DDA54A40-1E4C-11D1-A050-405705C10000" => "TOPOLOGY",  # TOPOLOGY
        "DFF220F3-F70F-11D0-B917-00A0C9223196" => "DMUSIC",    # SYNTHESIZER
        );

    print(STDERR "\t\tLooking for Interface: $Section.INTERFACES...\n");        

    if ( $inffile->sectionDefined("$Section.INTERFACES") ) {
        push(@Interfaces,Interfaces("$Section.INTERFACES"));
    }

 
    foreach $line (@lines) {
        $count++, next if ($line =~ /^\s*$/); # Skip if it's a blank line
        ($Directive,$Value) = ChkInf::SplitLine($Section,$count,$line);
        
        print (STDERR "\t\tDirective: $Directive, Value: $Value\n") if ($DEBUG);
        
        if (uc $Directive eq "INCLUDE") {
            push(@Include,( ChkInf::GetList($Value,$inffile->getIndex($Section,$count)) ));
        } elsif (uc $Directive eq "NEEDS") {
            push(@Needs,( ChkInf::GetList($Value,$inffile->getIndex($Section,$count)) ));
        } elsif (uc $Directive eq "ADDREG") {
            foreach ( ChkInf::GetList($Value,$inffile->getIndex($Section,$count)) ) {
                push(@RegLines, RegLines($_)) if ( $inffile->sectionDefined($_) );
            }
        }
    }

    foreach $Interface (@Interfaces) {
        $count = $$Interface[1];
        $line  = $$Interface[0];
        
        print(STDERR "\t\tInterface: $Interface\n") if ($DEBUG);
        
        ($GUID,$RefStr,$Sec,$Flags) = ChkInf::GetList($line,$count);
        $GUID   =~ s/{(.*)}/$1/;
        $GUID   =~ s/\"(.*)\"/$1/;
        $GUID   =  uc $GUID;
        $RefStr =~ s/\"(.*)\"/$1/;
        $RefStr =  uc $RefStr;
        
        print(STDERR "\t\t\t[$count]GUID: $GUID\n") if ($DEBUG);
        print(STDERR "\t\t\t[$count]RefStr: $RefStr\n") if ($DEBUG);
        print(STDERR "\t\t\t[$count]Sec: $Sec\n") if ($DEBUG);
        print(STDERR "\t\t\t[$count]Flags: $Flags\n") if ($DEBUG);

        if ($GUID eq "6994AD04-93EF-11D0-A3CC-00A0C9223196") { # KSCATEGORY_AUDIO
            $IsAudio++;
        }

        # find more AddReg items
        if (defined $Sec && $inffile->sectionDefined($Sec)) {
            @InterfaceLines = $inffile->getSection($Sec);
            
            foreach $InterfaceLine (@InterfaceLines) {
                $cIfaceLns++, next if ($InterfaceLine =~ /^\s*$/); # Skip if it's a blank line
                print(STDERR "\t\t\t[$cIfaceLns]InterfaceLine: $InterfaceLine") if ($DEBUG);
                
                ($Directive,$Value) = ChkInf::SplitLine($Sec,$cIfaceLns,$InterfaceLine);
                print (STDERR "\t\t\t\tDirective: $Directive, Value: $Value\n") if ($DEBUG);
                
                foreach ( ChkInf::GetList($Value,$inffile->getIndex($Sec,$cIfaceLns)) ) {
                    push(@RegLines, RegLines($_)) if ( $inffile->sectionDefined($_) );
                }
            }
        } 

        # The Audio Team no longer believes this is the case.
        #if (defined $KnownGUIDS{$GUID} ) {
        #    AddMediaError(8001,$count, $GUID, $KnownGUIDS{$GUID})
        #        unless ( $KnownGUIDS{$GUID} eq $RefStr );
        #}
    }
   
    if ($IsAudio) {
		unless ( grep( /KS.Registration/i, @Needs) ) {
			AddMediaError(8002,$inffile->getIndex($Section) - 1,"KS.Registration");
        }
        unless ( grep( /WDMAUDIO.Registration/i, @Needs) ) {
			AddMediaError(8002,$inffile->getIndex($Section) - 1,"WDMAUDIO.Registration");
        }
		unless ( grep( /ks.inf/i, @Include) ) {
			AddMediaError(8003,$inffile->getIndex($Section) - 1,"ks.inf");
        }
        unless ( grep( /wdmaudio.inf/i, @Include) ) {
			AddMediaError(8003,$inffile->getIndex($Section) - 1,"wdmaudio.inf");
        }
    }

    return 1;
}

#-------------------------------------------------------------------------------------------------#
# RegLines - returns all reglines (w/ string tokens removed) from a section.                      #
#-- RegLines() -----------------------------------------------------------------------------------#
sub RegLines {
    my $line;
    my $Section  =  shift;
    my @lines    =  $inffile->getSection($Section);
    my $count    =  0;
    my @reglines;
    my @rawRegLines;

    print(STDERR "\t\tInvoking Media::RegLines...\n") if ($DEBUG);
    print(STDERR "\t\t\tSection: $Section\n") if ($DEBUG);

    foreach $line (@lines) {
        #Skip blank lines even though this should never be true.
        $count++, next if ($line =~ /^[\s\t]*$/);
        push(@rawRegLines, [$line,$inffile->getIndex($Section,$count)]);
        # Remove all string tokens
        $line = ChkInf::RemoveStringTokens($inffile->getIndex($Section,$count),$line);
        # Update our list with [$line, $linenum]
        push(@reglines, [$line,$inffile->getIndex($Section,$count)]);
        $count++;
    }
    
    # This is the best chance to test the reglines in this section.
    # We need to use the raw reglines without strings removed for parsing to work correctly.
    # This is how setup works anyways.
    VerifyAddRegSection($Section, @rawRegLines);

    return(@reglines);
}


#-------------------------------------------------------------------------------------------------#
# VerifyAddRegSection - Verify the format of DeviceID is correct.                                 #
#-- VerifyAddRegSection() ------------------------------------------------------------------------#
sub VerifyAddRegSection {
    my $Section = shift;
    my @reglines = @_;
    my $count = 0;
    my $regline;
    my $regRoot;              # first part of a regline
    my $subkey;               # second part of a regline
    my $valEntryName;         # third part of a regline
    my $flags;                # fourth part of a regline
    my @values;               # the list of values for each reglist, value is considered to be anything but the first 4 parts
    my $value;                # each member of the value list
    my $cValues = 0;          # count of member of the value list
    my @endpoints;            # list of endpoints
    my $cEndpoints = 0;       # count of endpoints

    # print(STDERR "\t\tInvoking Media::VerifyAddRegSection...\n") if ($DEBUG);
    
    foreach $regline (@reglines) {
        $count++, next if ($$regline[0] =~ /^[\s\t]*$/);
        
        # we have to first parse out the string (without subsituting the strings)
        ($regRoot,$subkey,$valEntryName,$flags,@values) = split(/\s*,\s*/, $$regline[0]);
        
        # then we will replace the strings for each parsed out part to match the INF requirement
        $regRoot      = ChkInf::RemoveStringTokens($$regline[1], $regRoot);
        $subkey       = ChkInf::RemoveStringTokens($$regline[1], $subkey);
        $valEntryName = ChkInf::RemoveStringTokens($$regline[1], $valEntryName);
        $flags        = ChkInf::RemoveStringTokens($$regline[1], $flags);
        
                
        # let's count the association keys, i.e. those with PKEY_AudioEndpoint_Association as valEntryName
        if ($valEntryName eq "{1DA5D803-D492-4EDD-8C23-E0C0FFEE7F0E},2") {
            print(STDERR "\t\t\tregroot: $regRoot, subkey: $subkey, flags: $flags\n") if ($DEBUG);
            # we have one more endpoints now
            $cEndpoints++;
        
            VerifyHKRRegRoot($$regline[1], $regRoot);

            # verify that subkey is "EP\\x"
            if ($subkey =~ /^"EP\\\\([0-9]+)"$/) {
                if ($1 ne ($cEndpoints - 1)) { # there is a break in the sequency, and that's invalid
                    print(STDERR "!!ERROR 8007\n") if ($DEBUG);
                    AddMediaError(8007, $$regline[1], $subkey, $cEndpoints - 1);
                }
            }
            else
            {
                print(STDERR "!!ERROR 8006\n") if ($DEBUG);
                AddMediaError(8006, $$regline[1], $subkey);
            }
        }

        # if this is PKEY_AudioEngine_OEMPeriod
        if ($valEntryName eq "{E4870E26-3CC5-4CD2-BA46-CA0A9A70ED04},6") {
            print(STDERR "\t\t\tregroot: $regRoot, subkey: $subkey, flags: $flags\n") if ($DEBUG);
            
            VerifyHKRRegRoot($$regline[1], $regRoot);
            my $validValue = 1;
            $cValues = 0;
            foreach $value (@values) {
                print(STDERR "\t\t\tvalue[$cValues]: $value\n") if ($DEBUG);
                if (!($value =~ /^[0-9A-Fa-f]{2}$/)) {
                    print(STDERR "!!ERROR 8012 Invalid value($value)!\n") if ($DEBUG);
                    AddMediaError(8012, $$regline[1], $value);
                    $validValue = 0;
                } 
                $cValues++;
            }

            # make sure this is FLG_ADDREG_BINVALUETYPE (0x00000001)
            # and not FLG_ADDREG_TYPE_MULTI_SZ          (0x00010000)
            #     nor FLG_ADDREG_TYPE_EXPAND_SZ         (0x00020000)
            #     nor FLG_ADDREG_TYPE_DWORD             (0x00010001)
            #     nor FLG_ADDREG_TYPE_NONE              (0x00020001)
            # The above 4 invalid cases all have bit 16 and 17 set and none of other flags does
  
            if (!(hex($flags) & 0x1) or (hex($flags) & 0x00030000)) {
                print(STDERR "!!ERROR 8008\n") if ($DEBUG);
                AddMediaError(8008, $$regline[1], $flags);
            }
            
            if ($validValue) {
                
                # verify the value is a correct VT_BLOB
                # The first 8 bytes specifies the serialized data
                # for exampe, for this series (reading little endian): 41,00,63,00,08,00,00,00
                # 0041 - VT_BLOB
                # 0063 - stack noise produced by StgSerializePropVariant, so can be ignored
                # 00000008 - serialized data size. In the case Of OEM Periodicity it's always 8
                my $num = ConvertLittleEndianByteListToNumber(@values[0..1]); # byte 0 and byte 1
                if ($num ne hex(41)) {
                    print(STDERR "!!ERROR 8009 $num\n") if ($DEBUG);
                    AddMediaError(8009, $$regline[1], $num);
                }
                $num = ConvertLittleEndianByteListToNumber(@values[4..7]); # byte 4 to 7
                if ($num ne 8) {
                    print(STDERR "!!ERROR 8010 $num\n") if ($DEBUG);
                    AddMediaError(8010, $$regline[1], $num);
                }
                    
                $num = ConvertLittleEndianByteListToNumber(@values[8..15]); # the serialzed data
                if (($num ne 50000) and 
                    ($num ne 60000) and
                    ($num ne 70000) and
                    ($num ne 80000) and
                    ($num ne 90000)) {
                    print(STDERR "!!ERROR 8011 $num\n") if ($DEBUG);
                    AddMediaError(8011, $$regline[1], $num);
                }
            }
        }
    }
}


#-------------------------------------------------------------------------------------------------#
# ConvertLittleEndianByteListToNumber - Conver a list into HexNumber                              #
#-- ConvertLittleEndianByteListToNumber()---------------------------------------------------------#
sub ConvertLittleEndianByteListToNumber {
    my @bytes = reverse(@_);
    my $byte;
    my $num = 0;
    foreach $byte (@bytes) {
        $num *= 256;
        $num += hex($byte);
    }
    return $num;
}


#-------------------------------------------------------------------------------------------------#
# VerifyHKRRegRoot- Verify if regoot is HKR                                                       #
#-- VerifyHKRRegRoot() ---------------------------------------------------------------------------#
sub VerifyHKRRegRoot {
    my $index = shift;
    my $regRoot = shift;
    if ($regRoot ne "HKR") {
        print(STDERR "!!ERROR 8005\n") if ($DEBUG);
        AddMediaError(8005, $index, $regRoot);
    }
}

#-------------------------------------------------------------------------------------------------#
# VerifyDeviceID - Verify the format of DeviceID is correct.                                      #
#-- VerifyDeviceID() -----------------------------------------------------------------------------#
sub VerifyDeviceID {
    my $Section = shift;
    my $DeviceID = shift;

    my $UpperCaseDeviceID = uc $DeviceID;

    print(STDERR "\t\tInvoking Media::VerifyDeviceID($DeviceID)...\n") if ($DEBUG);

    my @DeviceIDParts = split(/\\/, $UpperCaseDeviceID);    

    # if it's an HD Audio device, we will verify it has more than two-part ID
    if ($DeviceIDParts[0] eq "HDAUDIO") {
        print(STDERR "\t\t\tVerify HDAUDIO PnP ID...\n") if ($DEBUG);
        
        if ($UpperCaseDeviceID =~    
             /^HDAUDIO\\
                 FUNC_([0-9A-Fa-f]{2})            # match FUNC_XX
                 &VEN_([0-9A-Fa-f]{4})            # match &VEN_YYYY
                 &DEV_([0-9A-Fa-f]{4})            # match &DEV_ZZZZ
                 (&SUBSYS_([0-9A-Fa-f]{8}))?      # match &SUBSYS_AAAAAAAA and SUBSYS is optional
                 (&REV_([0-9A-Fa-f]{4}))?         # match &REV_BBBB and REV is optional
              $/x) {                              # perl syntax allows the above to split into lines       
             print(STDERR "\t\t\tFUNC: $1, VEN: $2, DEV: $3, SUBSYS: $5, REV: $7\n") if ($DEBUG);
        } else {
             print(STDERR "\t\t\tInvalid HD Audio Device ID\n");
             AddMediaError(8004,$inffile->getIndex($Section) - 1, $DeviceID);
        }
    } 

    
    # BTHENUM\{0000110E-0000-1000-8000-00805F9B34FB} AVRCP is not tested with media class
    # HIDClass.pm is missing important code to get to this point of testing and that effort
    # is not worthwhile at this point given that the infrastructure is going to be updated.
    # We want the matching to be in this format:
    #   BTHENUM\{GGGGGGGG-GGGG-GGGG-GGGG-GGGGGGGGGGGG}_VID&TTTTVVVV_PID&XXXX
    #   - where {GGGGGGGG-GGGG-GGGG-GGGG-GGGGGGGGGGGG} is the service GUID
    #   - where TTTT is either 0001 (Bluetooth Vendor ID Type) or 0002 (USB Vendor ID Type)
    #   - where VVVV is the vendor ID
    #   - where XXXX is the product ID
    if ($DeviceIDParts[0] eq "BTHENUM") {
        print(STDERR "\t\t\tVerify Bluetooth PnP ID...\n") if ($DEBUG);
        # BTHENUM\{00001124-0000-1000-8000-00805f9b34fb}_VID&0002045e_PID&0700
        # now the second part of the device ID has the GUID and the rest
        if ($DeviceIDParts[1] =~ 
             /^(\{[0-9A-Fa-f]{8}-[0-9A-Fa-f]{4}-[0-9A-Fa-f]{4}-[0-9A-Fa-f]{4}-[0-9A-Fa-f]{12}\})
             _VID&([0-9A-Fa-f]{4})
                  ([0-9A-Fa-f]{4})
             _PID&([0-9A-Fa-f]+)
            $/x) {                               # perl syntax allows the above to split into lines    
            print(STDERR "\t\t\tGUID: $1, VID TYPE: $2, VID: $3, PID: $4\n") if ($DEBUG);
            if (hex $2 > 2 or hex $2 < 1)
            {
                print(STDERR"\t\t\tInvalid Bluetooth Device ID vendor ID Type ($2)\n") if ($DEBUG);
                AddMediaError(8014, $inffile->getIndex($Section) - 1, $2);
            }
        } else {
            print(STDERR "\t\t\tInvalid Bluetooth Audio Device ID\n") if ($DEBUG);
            AddMediaError(8013, $inffile->getIndex($Section) - 1, $DeviceID);
        }
    }
}
