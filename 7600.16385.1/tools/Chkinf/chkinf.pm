package ChkInf; {
use strict;

    sub Version          {return("v. 2.5")};    # Current version (used multiple places)
    sub CopyrightDates   {return("1998-2003")}; # Copyright (used multiple places)
    my $DEBUG = 0;

    no strict 'refs';    # we use pointers to functions
    use InfFile;         # Inf File object/methods

    # Global variables
    my $CurrentINF;      # INF currently being checked.

    my $ClassMod;        # Device class of this INF
    my $Module;          # Track current module being used
    my $DCOptions;       # Device Class specific options

    my %CheckedSections; # Track which sections we've used
    my %StringVals;      # Hash of all known strings
    my %Models;          # Models defined in current INF
    my %Globals;         # Suboptimal way for handling numerous globals.
                                 # $Globals{"ModelSection"}: contains the name of the current Model Section being processed

    my %ServiceInstallBin;	# A hash of hashes that keeps the name of the service binary 
    						# for each service name of a given model section.  
    						# The first key is the Model Section, 
    						# the second key the Service Name.
                                          # Example: 
                                          #      $ServiceInstallBin{"3COM.NT.5.1"}{"EL99X"} = EL99XN51.SYS


    my %ErrorTable;      # Listing of all errors found
    my $ErrorCount;      # Total number of errors found

    my %WarnTable;       # Listing of all warnings found
    my $WarnCount;       # Total number of warnings found

    my @CachedSDFiles = (); # SourceDisksFiles sections - cached per INF

    my %FilesChecked;    # Every file checked this invocation

    my $OSContext;       # Current OS to check for. "ALL" if all OSes
    my $INTERNAL;        # Are we running internally
    my $TotalLines;

    my %PnpIDList;       # Global list of all pnp-ids defined in the INF

    my $gCopyFilesCnt;   # used several places, so only calculate it once

    sub FALSE            {return(0);}   # For BOOLEAN testing
    sub TRUE             {return(1);}   # For BOOLEAN testing
    sub NUMERIC          { $a <=> $b; } # To sort numerically
    sub Mark             {$CheckedSections{uc($_[0])} = TRUE;}
    sub Mark2            {$CheckedSections{uc($_[0])} = $_[1];}
    sub IsMarked         {return $CheckedSections{uc($_[0])};}
    sub CreateLog        {$Globals{LOGFILE} = $_[0];}
    sub GetGlobalValue   {return $Globals{$_[0]};}

    sub is_numeric       { no strict;
                           my $var=shift;
                           return FALSE if ((~$var&$var)ne'0');
                           return TRUE;}


###################################################################################################
#+-----------------------------------------------------------------------------------------------+#
#+                                    MAIN ENTRY POINT                                           |#
#+-----------------------------------------------------------------------------------------------+#
###################################################################################################

#-------------------------------------------------------------------------------------------------
#-- ProcInf($inffile)
sub ProcInf {
    print("Starting ProcInf\n") if ($DEBUG);
    my($inffile) = $_[0];

    # this may result in a temp file being created to hold UTF8 data converted from a UNICODE INF
    $CurrentINF=InfFile->new($inffile);

    print("\tDone creating new inf. Handle is $CurrentINF\n") if ($DEBUG);
    
    # Don't check this inf
    unless ($CurrentINF->sectionDefined("Version")             &&   # This and
             ($CurrentINF->sectionDefined("Manufacturer")        || # One of these
              $CurrentINF->sectionDefined("DefaultInstall")      ||
              $CurrentINF->sectionDefined("DefaultInstall.NT")   ||
              $CurrentINF->sectionDefined("DefaultInstall.NTx86")||
              $CurrentINF->sectionDefined("DefaultInstall.NTia64")||
              $CurrentINF->sectionDefined("DefaultInstall.NTamd64"))) {
        AddErrorRef(1001, 0);
        print(STDERR "\tNot a valid driver INF.\n");
        return(FALSE);
    }

    my @Lines = ReadAllLines("$CurrentINF->{datafile}");
    # Total number of lines
    $TotalLines = @Lines + 1;

    # Do any files get copied?
    #### this count is also used in ProcSDFiles and ProcSDNames.
    $gCopyFilesCnt = grep(/^\s*CopyFiles\s*=/i,@Lines);

    # is there a copyright line?
    if ( grep(/Copyright/i,@Lines) < 1 ) {
        AddWarnRef(2212,0);
    }

    undef @Lines;

    my(%Models)=();


    # If we have a [STRINGS] section Read it Into Memory
    # Need to add error on no-strings-section-found
    if ( $CurrentINF->sectionDefined("STRINGS") &&
        ($CurrentINF->getOrigSection("STRINGS") > 0)) {
        LearnStrings("Strings");
    # Otherwise, look for a decorated strings section
    } else {
        my $sec;
        foreach $sec ($CurrentINF->getSections() ) {
           # by my understanding of PERL, the below pattern match is not what is desired
           if ( ($sec =~ /^STRINGS\.\w{4}$/i) ) {
                AddWarnRef(2100,$CurrentINF->getIndex($sec, 0),$sec);
                AddErrorRef(1003, 0, 'Strings');
                LearnStrings($sec);
                last; # Exit loop once one [Strings.*] section read
            }
        }
    }

    # If we have a [VERSION] section (and we should!) verify it
    if ( $CurrentINF->sectionDefined("Version") ) {
        ProcVersion("Version");
    } else {
        AddErrorRef(1003, 0, "Version");
    }

    #------------------------
    # Check all the standard sections
    #------------------------
    if ( $CurrentINF->sectionDefined("Manufacturer") ) {
        ProcManu("Manufacturer");
    } else {
        ##
        ## Handle DefaultInstall[.NT[x86|ia64|amd64]] before erroring
        ##
        if ($CurrentINF->sectionDefined("DefaultInstall")      ||
            $CurrentINF->sectionDefined("DefaultInstall.NT")   ||
            $CurrentINF->sectionDefined("DefaultInstall.NTx86")||
            $CurrentINF->sectionDefined("DefaultInstall.NTia64")||
            $CurrentINF->sectionDefined("DefaultInstall.NTamd64")) {

            if ( $CurrentINF->sectionDefined("DefaultInstall.NT")    ||
                 $CurrentINF->sectionDefined("DefaultInstall.NTx86") ||
                 $CurrentINF->sectionDefined("DefaultInstall.NTia64") ||
                 $CurrentINF->sectionDefined("DefaultInstall.NTamd64")) {
                # if specific extensions are defined, ignore undecorated
                # [DefaultInstall]
                ProcDefaultInstall("DefaultInstall.NT",".NT")
                    if ($CurrentINF->sectionDefined("DefaultInstall.NT"));
                ProcDefaultInstall("DefaultInstall.NTx86",".NTx86")
                    if ($CurrentINF->sectionDefined("DefaultInstall.NTx86"));
                ProcDefaultInstall("DefaultInstall.NTia64",".NTia64")
                    if ($CurrentINF->sectionDefined("DefaultInstall.NTia64"));
                    ProcDefaultInstall("DefaultInstall.NTamd64",".NTamd64")
                        if ($CurrentINF->sectionDefined("DefaultInstall.NTamd64"));
            } else {
                # [DefaultInstall] must be defined
                ProcDefaultInstall("DefaultInstall","");
            }

        } else {
            AddErrorRef(1003, 0, "Manufacturer");
        }
    }

    if ( $CurrentINF->sectionDefined("ControlFlags") ) {
        ProcCFlagsSection("ControlFlags");
    }

    #------------------------
    # Verify Class Installers - Need to verify if .ext models exist
    #------------------------
    ProcClassInstall32Section("CLASSINSTALL32")
        if ( $CurrentINF->sectionDefined("ClassInstall32") );
    ProcClassInstall32Section("CLASSINSTALL32.NT")
        if ( $CurrentINF->sectionDefined("ClassInstall32.NT") );
    ProcClassInstall32Section("CLASSINSTALL32.NTX86")
        if ( $CurrentINF->sectionDefined("ClassInstall32.NTX86") );
    ProcClassInstall32Section("CLASSINSTALL32.NTIA64")
        if ( $CurrentINF->sectionDefined("ClassInstall32.NTIA64") );
    ProcClassInstall32Section("CLASSINSTALL32.NTAMD64")
        if ( $CurrentINF->sectionDefined("ClassInstall32.NTAMD64") );

    #------------------------
    # Verify SourceDisksNames - Need to verify if .ext models exist
    #------------------------
    ProcSDNames("SOURCEDISKSNAMES")       if ( $CurrentINF->sectionDefined("SourceDisksNames") );
    ProcSDNames("SOURCEDISKSNAMES.NT")    if ( $CurrentINF->sectionDefined("SourceDisksNames.NT") );
    ProcSDNames("SOURCEDISKSNAMES.X86")   if ( $CurrentINF->sectionDefined("SourceDisksNames.X86") );
    ProcSDNames("SOURCEDISKSNAMES.IA64")  if ( $CurrentINF->sectionDefined("SourceDisksNames.IA64") );
    ProcSDNames("SOURCEDISKSNAMES.AMD64") if ( $CurrentINF->sectionDefined("SourceDisksNames.AMD64") );

    # Commonly mis-decorated sections
    AddErrorRef(1085,$CurrentINF->getIndex("SourceDisksNames.NTx86",0)-1,"NTX86","SourceDisksNames")
            if ( $CurrentINF->sectionDefined("SourceDisksNames.NTX86") );
    AddErrorRef(1085,$CurrentINF->getIndex("SourceDisksNames.NTIA64",0)-1,"NTIA64","SourceDisksNames")
            if ( $CurrentINF->sectionDefined("SourceDisksNames.NTIA64") );
    AddErrorRef(1085,$CurrentINF->getIndex("SourceDisksNames.NTAMD64",0)-1,"NTAMD64","SourceDisksNames")
            if ( $CurrentINF->sectionDefined("SourceDisksNames.NTAMD64") );
    AddErrorRef(1085,$CurrentINF->getIndex("SourceDisksNames.NTAlpha",0)-1,"NTALPHA","SourceDisksNames")
            if ( $CurrentINF->sectionDefined("SourceDisksNames.NTALPHA") );
    AddErrorRef(1085,$CurrentINF->getIndex("SourceDisksNames.Alpha",0)-1,"ALPHA","SourceDisksNames")
            if ( $CurrentINF->sectionDefined("SourceDisksNames.ALPHA") );

    if ( grep(/^SourceDisksNames/i, $CurrentINF->getSections()) ) {
        unless ( grep(/^SourceDisksFiles/i, $CurrentINF->getSections() ) ) {
            AddErrorRef(1003, 0, 'SourceDisksFiles') if (($gCopyFilesCnt) && (! $INTERNAL));
        }
    } else {
        AddErrorRef(1003, 0, 'SourceDisksNames') if (($gCopyFilesCnt) && (! $INTERNAL));
    }

    #------------------------
    # Verify SourceDisksFiles - Need to verify if .ext models exist
    #------------------------
    ProcSDFiles("SOURCEDISKSFILES")       if ( $CurrentINF->sectionDefined("SourceDisksFiles") );
    ProcSDFiles("SOURCEDISKSFILES.X86")   if ( $CurrentINF->sectionDefined("SourceDisksFiles.X86") );
    ProcSDFiles("SOURCEDISKSFILES.IA64")  if ( $CurrentINF->sectionDefined("SourceDisksFiles.IA64") );
    ProcSDFiles("SOURCEDISKSFILES.AMD64") if ( $CurrentINF->sectionDefined("SourceDisksFiles.AMD64") );

    # Commonly mis-decorated section
    AddErrorRef(1085,$CurrentINF->getIndex("SourceDisksFiles.NTx86",0)-1,"NTX86","SourceDisksFiles")
            if ( $CurrentINF->sectionDefined("SourceDisksFiles.NTX86") );
    AddErrorRef(1085,$CurrentINF->getIndex("SourceDisksFiles.NTIA64",0)-1,"NTIA64","SourceDisksFiles")
            if ( $CurrentINF->sectionDefined("SourceDisksFiles.NTIA64") );
    AddErrorRef(1085,$CurrentINF->getIndex("SourceDisksFiles.NTAMD64",0)-1,"NTAMD64","SourceDisksFiles")
            if ( $CurrentINF->sectionDefined("SourceDisksFiles.NTAMD64") );
    AddErrorRef(1085,$CurrentINF->getIndex("SourceDisksFiles.NTAlpha",0)-1,"NTALPHA","SourceDisksFiles")
            if ( $CurrentINF->sectionDefined("SourceDisksNames.NTALPHA") );
    AddErrorRef(1085,$CurrentINF->getIndex("SourceDisksFiles.Alpha",0)-1,"ALPHA","SourceDisksFiles")
            if ( $CurrentINF->sectionDefined("SourceDisksFiles.ALPHA") );

    #--------------------------
    # Verify InterfaceInstall32
    #--------------------------
    ProcInterfaceInstall32Section("INTERFACEINSTALL32")
            if ( $CurrentINF->sectionDefined("InterfaceInstall32") );

    # We need to check this last so we can verify that the sections are referenced
    #       by a CopyFiles, RenFiles, or DelFiles
    #--------------------------
    # Verify DestinationDirs
    #--------------------------
    if ( $CurrentINF->sectionDefined("DestinationDirs") ) {
        ProcDestDirsSection("DestinationDirs");
    } else {
        AddErrorRef(1003, 0, 'DestinationDirs') if ($gCopyFilesCnt);
    }

    if (! defined($Module) ) {
        warn("The variable \$Module is undefined!\n") if ($DEBUG);
        return(FALSE);
    } else {
        $Module->InfCheck();
    }

    # Cross-check all of the [strings.*] sections
    CheckStringsWithLangID();

    # Need to find unreferenced sections & invalid section extensions
    foreach ( $CurrentINF->getSections() ) {
        AddWarnRef(2083, $CurrentINF->getIndex($_, 0)-1,$_ )
                unless ( defined $CheckedSections{uc($_)} );
        AddErrorRef(1085,$CurrentINF->getIndex($_)-1,"PPC"  ,$1) if (m/(.*?)\.PPC$/i);
        AddErrorRef(1085,$CurrentINF->getIndex($_)-1,"Mips" ,$1) if (m/(.*?)\.MIPS$/i);
        AddErrorRef(1085,$CurrentINF->getIndex($_)-1,"Alpha",$1) if (m/(.*?)\.ALPHA$/i);
        AddErrorRef(1085,$CurrentINF->getIndex($_)-1,"AXP64",$1) if (m/(.*?)\.AXP64$/i);
    }

    # Test for DOS-style EOL
    open(hFILE, $inffile);
    binmode(hFILE);
    my($l) = <hFILE>;
    close hFILE;

    my(@uni) = unpack("C*",$l);
    # looking for \r\n
    AddWarnRef(2205, 0) if ($uni[$#uni-1] != 13 and $uni[$#uni] != 10);
    undef @uni;
    undef $l;

    # Verify that filters aren't controlling services
    my ($filter, $binary, $service);
    # Binary
    foreach $filter ( (@{$Globals{"UPPERFILTERS"}},@{$Globals{"LOWERFILTERS"}}) ) {
        # Binary, ServiceSection
        #foreach $binary (@{$Globals{"ServiceBinaries"}}) {
        #   if (uc ${$binary}[2] eq uc $filter) {
        #       print "-- ${$binary}[0] ${$binary}[1]  ${$binary}[2] ($filter)\n";
        #       $Globals{"CheckService"}= ${$binary}[1];
        #   }
        #}
        # Section, Flag, LineNumber
        foreach $service (@{$Globals{"ServiceSectionFlags"}}) {
            if ((uc ${$service}[0] eq uc $filter) &&
                (${$service}[1] == 2)) {
                AddErrorRef(1302, ${$service}[2]);
            }
        }

    }
    return(TRUE);
}

###################################################################################################
#+-----------------------------------------------------------------------------------------------+#
#|                               INF Checking Routines                                            #
#+-----------------------------------------------------------------------------------------------+#
###################################################################################################

#-------------------------------------------------------------------------------------------------
#-- ProcDefaultInstall($Section)
sub ProcDefaultInstall {
    print("Starting ProcDefaultInstall\n") if ($DEBUG);

    my ($Section)      = shift;
    my ($Extension)    = shift;
    my (@lines)        = $CurrentINF->getSection($Section);
    my ($line);
    my ($SecStart)     = $CurrentINF->getIndex($Section,0)-1;

    my(%DefaultInstallDirective) = ("ADDREG",           1,
                                    "BITREG",           1,
                                    "COPYFILES",        1,
                                    "COPYINF",          1,
                                    "DELFILES",         1,
                                    "DELREG",           1,
                                    "DRIVERVER",        1,
                                    "INCLUDE",          1,
                                    "INI2REG",          1,
                                    "NEEDS",            1,
                                    "OPTIONDESC",       1,
                                    "PROFILEITEMS",     1,
                                    "REBOOT",           1,
                                    "REGISTERDLLS",     1,
                                    "RENFILES",         1,
                                    "UNREGISTERDLLS",   1,
                                    "UPDATEINIS",       1,
                                    "UPDATEINIFIELDS",  1);

    my ($Directive, $Value, @Values);
    my ($count)        = 0;

    foreach $line (@lines) {

        $count++, next if ($line =~ /^[\s\t]*$/);# Skip if it's a blank line

        ($Directive, $Value) = SplitLine($Section,$count,$line);

        $Directive = uc $Directive;

        if (! defined $DefaultInstallDirective{$Directive} ) {
            AddWarnRef(2202, $CurrentINF->getIndex($Section,$count), $Directive);
            next;
        } else {
            ##-- Verify the directive
                        $Globals{CONTEXT} = "DEFAULTINSTALL";
            &$Directive($Section, $count, $Value) unless (uc $Directive eq "OPTIONDESC");
                        undef $Globals{CONTEXT};
        }
    }

    if ( $CurrentINF->sectionDefined("$Section.SERVICES") ) {
        ProcInstallServicesSection("$Section.SERVICES");
    }

    if ( $CurrentINF->sectionDefined("DEFAULTUNINSTALL${Extension}") ) {
        ProcDefaultUninstall("DEFAULTUNINSTALL${Extension}");
    }

    if ($CurrentINF->sectionDefined($Section.".Wdf")) {
        ProcWdfSection($Section);
    }
    $CheckedSections{uc($Section)} = TRUE;

    
    return TRUE;
}

#-------------------------------------------------------------------------------------------------
#-- ProcDefaultUninstall($Section)
sub ProcDefaultUninstall {
    print("Starting ProcDefaultUninstall\n") if ($DEBUG);

    my ($Section)      = shift;
    my ($Extension)    = shift;
    my (@lines)        = $CurrentINF->getSection($Section);
    my ($line);
    my ($SecStart)     = $CurrentINF->getIndex($Section,0)-1;

    my(%DefaultInstallDirective) = ("DELREG",           1,
                                    "DELFILES",         1,
                                    "DRIVERVER",        1,
                                    "INCLUDE",          1,
                                    "NEEDS",            1,
                                    "REBOOT",           1);


    my ($Directive, $Value, @Values);
    my ($count)        = 0;

   foreach $line (@lines) {

        $count++, next if ($line =~ /^[\s\t]*$/);# Skip if it's a blank line

        ($Directive, $Value) = SplitLine($Section,$count,$line);
        $Directive = uc $Directive;

        if (! defined $DefaultInstallDirective{uc $Directive} ) {
            AddWarnRef(2202, $CurrentINF->getIndex($Section,$count), $Directive);
            next;
        } else {
            ##-- Verify the directive
            &$Directive($Section, $count, $Value);
        }
    }

    if ( $CurrentINF->sectionDefined("$Section.SERVICES") ) {
        ProcUninstallServicesSection("$Section.SERVICES");
    }

    $CheckedSections{uc($Section)} = TRUE;
    return TRUE;
}

#-------------------------------------------------------------------------------------------------
#-- ProcManu($Section)
sub ProcManu {
    print("Starting ProcManu\n") if ($DEBUG);
    my ($Section)      = shift;
    my (@lines)        = $CurrentINF->getSection($Section);
    my ($line);
    my ($SecStart)     = $CurrentINF->getIndex($Section,0)-1;

    my ($Directive, $Value, @Values);
    my ($count)        = 0;
    my (%ManuList);
    my ($NTARCH,$OSMaj,$OSMin,$OSType,$Suite);

   foreach $line (@lines) {

        $count++, next if ($line =~ /^[\s\t]*$/);# Skip if it's a blank line

        ($Directive, $Value) = SplitLine($Section,$count,$line);

        # Check to see if we have already seen this manufacturer-identifier
        if ( exists $ManuList{uc($Directive)}) {
           AddErrorRef(1317,$CurrentINF->getIndex($Section,$count), $Directive);
           next;
        }

        # Whistler allows new TargetDecorations.
        @Values = GetList($Value,$CurrentINF->getIndex($Section, $count));
        # Value holds the undecorated section, @Values holds TargetDecorations.
        $Value  = shift(@Values);

        # [Manufacturers] may not have both a directive & a value
        if ((! defined $Value) or ($Value eq "")) {
            # It's just a model, not a manufacturer
            $Value = $Directive;
            $ManuList{uc($Directive)} = "none-$Value";
        } else {
            $ManuList{uc($Directive)} = "$Value";
        }

        $Value =~ s/\"//g;

        if ( $CurrentINF->sectionDefined($Value) ) {
            ProcModelSection($Value);
        } else {
            # INF Section for this model isn't defined.
            AddErrorRef(1003,$CurrentINF->getIndex($Section,$count), $Value) unless (@Values >= 1);
        }

        foreach (@Values) {

            ($NTARCH,$OSMaj,$OSMin,$OSType,$Suite) = split(/\./,$_);
            # $NTARCH NT, NTx86, NTIA64, NTAMD64
            # $OSMaj is the OS major version (e.g., for Whistler, it's 5).
            # $OSMin is the OS minor version (e.g., for Whistler, it's 1).
            # $OSType indicates the type of product, and may be one of the following values (as defined in winnt.h):
            #   VER_NT_WORKSTATION              0x0000001
            #   VER_NT_DOMAIN_CONTROLLER        0x0000002
            #   VER_NT_SERVER                   0x0000003
            # If multiple product types are supported (i.e., some subset of all product types, such as domain
            # controllers and servers, but not workstations), then multiple decorations must be specified, one for 
            # each applicable product type.
            # $Suite is a combination of the following flags identifying the product suites available on the system
            # (as defined in winnt.h):
            #   VER_SUITE_SMALLBUSINESS             0x00000001
            #   VER_SUITE_ENTERPRISE                0x00000002
            #   VER_SUITE_BACKOFFICE                0x00000004
            #   VER_SUITE_COMMUNICATIONS            0x00000008
            #   VER_SUITE_TERMINAL                  0x00000010
            #   VER_SUITE_SMALLBUSINESS_RESTRICTED  0x00000020
            #   VER_SUITE_EMBEDDEDNT                0x00000040
            #   VER_SUITE_DATACENTER                0x00000080
            #   VER_SUITE_SINGLEUSERTS              0x00000100
            if ( $CurrentINF->sectionDefined("$Value.$_") ) {
                ProcModelSection("$Value.$_");
            } else {
                AddErrorRef(1003, $CurrentINF->getIndex($Section,$count), "$Value.$_");
            }
        }
        $count++;
    }

    if ( keys(%ManuList) <= 0 ) {
       # Must have at least one manufacturer.
       AddErrorRef(1039, $SecStart, $Section);
    }

    $CheckedSections{uc($Section)} = TRUE;
    return(TRUE);
}

#-----------------------------------------------------------------------------------------------------
#-- ProcCFlagsSection($Section)
sub ProcCFlagsSection {
    print("Starting ProcCFlagsSection\n") if ($DEBUG);
    my($Section) =  $_[0];
    my(@lines)    = $CurrentINF->getSection($Section);
    my($SecStart) = $CurrentINF->getIndex($Section,0)-1;
    my($line);
    my($count)=0;

    my($Directive, $Value, @Values);
    my(%CFlagKeys)=('EXCLUDEFROMSELECT',        'ExSel',
                    'EXCLUDEFROMSELECT.NT',     'ExSel',
                    'EXCLUDEFROMSELECT.NTX86',  'ExSel',
                    'EXCLUDEFROMSELECT.NTIA64', 'ExSel',
                    'EXCLUDEFROMSELECT.NTAMD64','ExSel',
                    'INTERACTIVEINSTALL',       ' ',
                    'COPYFILESONLY',            ' ');

    foreach $line (@lines) {
        $count++, next if ($line =~ /^[\s\t]*$/); # Skip if it's a blank line

        ($Directive, $Value) = SplitLine($Section,$count,$line);

        # $Directive should be one of the CFlagKeys.
        if (!defined ($CFlagKeys{uc($Directive)})) {
            if ($Module eq "GENERIC") {
                AddWarnRef(2202, $CurrentINF->getIndex($Section, $count),
                    "$Directive (may be device class specific)");
            } else {
                AddWarnRef(2202, $CurrentINF->getIndex($Section,$count), $Directive)
                    unless (($Directive =~ /^[\s\t]*$/) || $Module->DCDirective($Directive));
            }

            $count++;
            next;
        }

        if ($Directive =~ /^EXCLUDEFROMSELECT/i) {
            @Values = GetList($Value,$CurrentINF->getIndex($Section, $count));
            foreach $Value (@Values) {
                # If the model name is not '*' (which is allowed), check all hw-id and compat-id
                # listed in the INF and log an error if this is not found in the list.
                if ($Value !~ /\*/) {
                    AddErrorRef(1072, $CurrentINF->getIndex($Section,$count), $Value)
                       unless (defined $PnpIDList{uc($Value)});
                }
           }
        #-------------------------------------------------------------#
        # This section should check the ExcludeFromSelect directive   #
        #-------------------------------------------------------------#
        }
        if ($Directive =~ /^INTERACTIVEINSTALL$/i) {

            @Values = GetList($Value,$CurrentINF->getIndex($Section,$count));

            foreach $Value (@Values) {
                AddErrorRef(1072, $CurrentINF->getIndex($Section,$count), $Value)
                    unless (($Value eq "*") || defined $PnpIDList{uc($Value)});
           }
        }
    $count++;
    }

    $CheckedSections{uc($Section)} = TRUE;

    return(TRUE);
}

#-------------------------------------------------------------------------------------------------
#-- ProcClassInstall32Section($Section)
sub ProcClassInstall32Section {
        # HKR = HKLM,System\CurrentControlSet\Control\Class\{SetupClassGUID}
    print("Starting ProcClassInstall32Section\n") if ($DEBUG);
    my($Section) =  $_[0];
    my(@lines)    = $CurrentINF->getSection($Section);
    my($SecStart) = $CurrentINF->getIndex($Section,0)-1;
    my($line);
    my($count)=0;

    my($Directive, $Value);
    my($AddReg) = 0;
    my(%ClassInstall32Directive) = ("ADDREG",           1,
                                    "BITREG",           1,
                                    "COPYFILES",        1,
                                    "DELFILES",         1,
                                    "DELREG",           1,
                                    "INI2REG",          1,
                                    "RENFILES",         1,
                                    "UPDATEINIS",       1,
                                    "UPDATEINIFIELDS",  1);

    if (defined $CheckedSections{uc($Section)} ) {
        return(FALSE);
    } else {

        foreach $line (@lines) {
            $count++, next if ($line =~ /^[\s\t]*$/);# Skip if it's a blank line

            ($Directive, $Value) = SplitLine($Section,$count,$line);

            unless ( defined($ClassInstall32Directive{uc($Directive)}) )  {
                if ($Module eq "GENERIC") {
                    AddWarnRef(2202, $CurrentINF->getIndex($Section, $count),
                        "$Directive (may be device class specific)");
                } else {
                    AddWarnRef(2202, $CurrentINF->getIndex($Section, $count), $Directive)
                        unless ($Module->DCDirective($Directive));
                }

                $count++;
                next;
            }

                        $Globals{CONTEXT} = "CLASSINSTALL32";
            &{uc($Directive)}($Section, $count, $Value);
                        undef $Globals{CONTEXT};

            $AddReg++ if (uc($Directive) eq "ADDREG");
            $count++;
        }

        AddErrorRef(1058,$CurrentINF->getIndex($Section, $count),"AddReg",$Section) unless ($AddReg);

        $CheckedSections{uc($Section)} = TRUE;
    }

    return(TRUE);
}

#-----------------------------------------------------------------------------------------------------
#   This sub processes the DestinationDirs section and retains the list of file sections defined
#-- ProcDestDirsSection($Section)
sub ProcDestDirsSection {
    print("Starting ProcDestDirsSection\n") if ($DEBUG);
    my($Section) =  $_[0];
    my(@lines)    = $CurrentINF->getSection($Section);
    my($SecStart) = $CurrentINF->getIndex($Section,0)-1;
    my($line);
    my($count)=0;

    my($Directive, $Value);
    my(%DestinationDirs);
    my($DDDirs)         = FALSE;
    my(@FileSectValue);


    foreach $line (@lines) {

        ($count++, next) if ($line =~ /^[\s\t]*$/); # Skip if it's a blank line

        ($Directive, $Value) = SplitLine($Section,$count,$line);

        if (defined($DestinationDirs{uc($Directive)}))  {
            $count++;
            AddErrorRef(1042, $CurrentINF->getIndex($Section,$count));
            next;
        }

        $DestinationDirs{uc($Directive)} = $Value;

        if ($Directive !~ /^DefaultDestDir$/i) {
            if ( $CurrentINF->sectionDefined($Directive) ) {
                unless (defined $CurrentINF->sectionDefined($Directive) ) {
                    AddWarnRef(2147, $CurrentINF->getIndex($Section,$count), $Directive)
                        if ($CurrentINF->getOrigSection($Directive));
                }
            } else {
                AddWarnRef(2003, $CurrentINF->getIndex($Section,$count), $Directive);
            }
        } else {
            $DDDirs = TRUE;
        }

        @FileSectValue = GetList($Value,$CurrentINF->getIndex($Section,$count));

        ##
        ## Verify this code
        ##
        if (!defined($FileSectValue[0])) {
            AddErrorRef(1043, $CurrentINF->getIndex($Section,$count));
        } elsif (!defined($StringVals{$FileSectValue[0]})) {
            if ($FileSectValue[0] =~ /^\d*$/) {
                AddErrorRef(1044, $CurrentINF->getIndex($Section,$count))
                                         unless (($FileSectValue[0] >= 66000) and
                                                 ($FileSectValue[0] <= 66004) and
                                                 (uc($Module) eq "PRINTER"));
            } else {
                AddErrorRef(1071, $CurrentINF->getIndex($Section,$count),
                                "\"$FileSectValue[0]\"", "DirID");
            }
        }
        $count++;
    }
#    AddWarnRef(2020, $SecStart, "DefaultDestDir", $Section) unless ($DDDirs);

    $CheckedSections{uc($Section)} = TRUE;

    return(TRUE);
}


#-------------------------------------------------------------------------------------------------
#-- ProcWMISecuritySection($Section)
sub ProcWMISecuritySection {
   #WMIInterface=<guid>,flags,<section name>

    print("Starting ProcWMISecuritySection\n") if ($DEBUG);

    my($Section) =  $_[0];
    my(@lines)    = $CurrentINF->getSection($Section);
    my($SecStart) = $CurrentINF->getIndex($Section,0)-1;
    my($line);
    my($count)=0;
    my($guid);
    my($flags);
    my($sectionName);

    my($Directive, $Value);
    my(%WMISecuritySectionDirectives) =("WMIINTERFACE",          0,);

    if (defined $CheckedSections{uc($Section)})  {
        return(FALSE);
    } else {

        foreach $line (@lines) {
            $count++, next if ($line =~ /^[\s\t]*$/);# Skip if it's a blank line

            ($Directive, $Value) = SplitLine($Section,$count,$line);
            if (!defined($WMISecuritySectionDirectives{uc($Directive)})) {
               AddErrorRef(1321,$CurrentINF->getIndex($Section, $count),"$Directive");
            }
            else {
               if ($Value =~ /\A([^\,]*)\,([^\,]*)\,([^\,]*)\z/) {
                  ($guid = $1) =~ s/\s+//g;
                  ($flags = $2) =~ s/\s+//g;
                  ($sectionName = $3) =~ s/\s+//g;

                  # braces around the guid are optional, so add them if not present
                  if ($guid !~ /\A\{/ && $guid !~ /\}\z/) {
                     $guid = "{" . $guid . "}";
                  }
                  if (($guid !~ /\{.{8}-.{4}-.{4}-.{4}-.{12}\}/)) {
                     AddErrorRef(1323,$CurrentINF->getIndex($Section, $count));
                  }
                  if (length($flags) != 0 && $flags ne "1") {
                     AddErrorRef(1324,$CurrentINF->getIndex($Section, $count));
                  }
                  if (length($sectionName) == 0) {
                     AddErrorRef(1325,$CurrentINF->getIndex($Section, $count));
                  }
                  else {
                     if (!defined($CurrentINF->{uc($sectionName)}->[0])) {
                        AddErrorRef(1328,$CurrentINF->getIndex($Section, $count));
                     }
                     else {
                        ProcWMISecurityDirective($sectionName);
                     }
                  }
               }
               else {
                  AddErrorRef(1322,$CurrentINF->getIndex($Section, $count),"$Value");
               }

            }
        }
        $CheckedSections{uc($Section)} = TRUE;
    }
}


#-------------------------------------------------------------------------------------------------
#-- ProcWMISecurityDirective($Section)
sub ProcWMISecurityDirective {
   #Security="<security descriptor>"
   print("Starting ProcWMISecuritySection\n") if ($DEBUG);

   my($Section) =  $_[0];
   my(@lines)    = $CurrentINF->getSection($Section);
   my($SecStart) = $CurrentINF->getIndex($Section,0)-1;
   my($line);
   my($count)=0;
   my($directiveCount)=0;
   my($guid);
   my($flags);
   my($sectionName);

   my($Directive, $Value);

   if (defined $CheckedSections{uc($Section)})  {
       return(FALSE);
   } else {

       foreach $line (@lines) {
           $count++, next if ($line =~ /^[\s\t]*$/);# Skip if it's a blank line
           ($Directive, $Value) = SplitLine($Section,$count,$line);

           if (uc($Directive) ne "SECURITY") {
              AddErrorRef(1326,$CurrentINF->getIndex($Section, $count));
           }
           else {
              $directiveCount++;
           }
           if ($directiveCount >1 ) {
              AddErrorRef(1327,$CurrentINF->getIndex($Section, $count));
           }
           CallChkSD($Section, $count, $Value);
       }
       $CheckedSections{uc($Section)} = TRUE;
   }
}

#-------------------------------------------------------------------------------------------------
#-- ProcInstallCoInstallersSection($Section)
sub ProcInstallCoInstallersSection {
        # HKR = HKLM,System\CurrentControlSet\Control\Class\{%SetupClassGUID%}\{device-instance-id}
    print("Starting ProcInstallCoInstallersSection\n") if ($DEBUG);
    my($Section) =  $_[0];
    my(@lines)    = $CurrentINF->getSection($Section);
    my($SecStart) = $CurrentINF->getIndex($Section,0)-1;
    my($line);
    my($count)=0;

    my($Directive, $Value);
    my(%InstallCoInstallersDirective) =("ADDREG",          0,
                                        "BITREG",          0,
                                        "COPYFILES",       0,
                                        "DELFILES",        0,
                                        "DELREG",          0,
                                        "INI2REG",         0,
                                        "RENFILES",        0,
                                        "UPDATEINIS",      0,
                                        "UPDATEINIFIELDS", 0,
                                        "INCLUDE",         0,
                                        "NEEDS",           0);

    if (defined $CheckedSections{uc($Section)})  {
        return(FALSE);
    } else {

        foreach $line (@lines) {
            $count++, next if ($line =~ /^[\s\t]*$/);# Skip if it's a blank line

            ($Directive, $Value) = SplitLine($Section,$count,$line);

           unless (defined($InstallCoInstallersDirective{uc($Directive)}))  {
                if ($Module eq "GENERIC") {
                    AddWarnRef(2202, $CurrentINF->getIndex($Section, $count),
                        "$Directive (may be device class specific)");
                } else {
                    AddWarnRef(2202, $CurrentINF->getIndex($Section, $count), $Directive)
                        unless ($Module->DCDirective($Directive));
                }
                $count++;
                next;
            }

            $InstallCoInstallersDirective{uc($Directive)} = TRUE;
                        $Globals{CONTEXT} = "DDINSTALL.COINSTALLERS";
            &{uc($Directive)}($Section, $count, $Value);
                        undef $Globals{CONTEXT};

            $count++;
        }

        # If a coinstaller install section has a NEEDS directive, we assume that the coinstaller binaries are imported
        # onto the system from the referenced section. Hence, there is no need to have a CopyFiles directive or an
        # AddReg directive in this section. This may not be necessarily true in cases where there are more than one coinstallers, but it covers 
        # most of the cases.
        AddErrorRef(1058,$SecStart,"AddReg",$Section)
                            unless ($InstallCoInstallersDirective{"ADDREG"} || $InstallCoInstallersDirective{"NEEDS"});
        AddErrorRef(1058,$SecStart,"CopyFiles",$Section)
                            unless ($InstallCoInstallersDirective{"COPYFILES"} || $InstallCoInstallersDirective{"NEEDS"});
        $CheckedSections{uc($Section)} = TRUE;
    }

    return(TRUE);
}

#-------------------------------------------------------------------------------------------------
#-- ProcInstallHWSection($Section)
sub ProcInstallHWSection {
        # HKR = ..Enum\enumeratorID\device-instance-id
        # HKR = HKLM,System\CurrentControlSet\ENUM\Model??\DevInstanceID
        print("Starting ProcInstallHWSection\n") if ($DEBUG);
    my($Section) =  $_[0];
    my(@lines)    = $CurrentINF->getSection($Section);
    my($SecStart) = $CurrentINF->getIndex($Section,0)-1;
    my($line);
    my($count)=0;

    my($Directive, $Value);
    my(%InstallHWDirective) = ("ADDREG",  1,
                               "BITREG",  1,
                               "DELREG",  1,
                               "INCLUDE", 1,
                               "NEEDS",   1);

    if ( defined $CheckedSections{uc($Section)} )  {
        return(FALSE);
    } else {

        foreach $line (@lines) {
            $count++, next if ($line =~ /^[\t\s]*$/);# Skip if it's a blank line

            ($Directive, $Value) = SplitLine($Section,$count,$line);

            unless (defined($InstallHWDirective{uc($Directive)}))  {
                if ($Module eq "GENERIC") {
                    AddWarnRef(2202, $CurrentINF->getIndex($Section, $count),
                        "$Directive (may be device class specific)");
                } else {
                    AddWarnRef(2202, $CurrentINF->getIndex($Section, $count), $Directive)
                        unless ($Module->DCDirective($Directive));
                }
                $count++;
                next;
            }
                        $Globals{CONTEXT} = "DDINSTALL.HW";
            &{uc($Directive)}($Section,$count,$Value);
                        undef $Globals{CONTEXT};
            $count++;
        }
        $CheckedSections{uc($Section)} = TRUE;
    }
    return(TRUE);
}

#-------------------------------------------------------------------------------------------------
#-- ProcInstallFactDefSection($Section)
sub ProcInstallFactDefSection {
    print("Starting ProcInstallFactDefSection\n") if ($DEBUG);
    my($Section) =  $_[0];
    my(@lines)    = $CurrentINF->getSection($Section);
    my($SecStart) = $CurrentINF->getIndex($Section,0)-1;
    my($line);
    my($count)=0;

    my($Directive, $Value, $Attribs, $Num);
    my($ConfigPriority) = 0;
    my(%FactDefDirectives) = ("CONFIGPRIORITY",0,
                              "DMACONFIG",     0,
                              "IOCONFIG",      0,
                              "MEMCONFIG",     0,
                              "IRQCONFIG",     0
                             );

    if ( defined $CheckedSections{uc($Section)} )  {
        return(FALSE);
    } else {

        foreach $line (@lines) {
            $count++, next if ($line =~ /^[\s\t]*$/);# Skip if it's a blank line

            ($Directive, $Value) = SplitLine($Section,$count,$line);

            if (uc($Directive) eq "CONFIGPRIORITY") {
                if ($FactDefDirectives{uc($Directive)} eq 0) {
                    $FactDefDirectives{uc($Directive)} = $Value;
                    if ($Value ne "HARDRECONFIG") {
                        AddErrorRef(1066,$CurrentINF->getIndex($Section, $count),
                            "ConfigPriority (expected \"HardReconfig\")",$Section);
                    }
                } else {
                    AddErrorRef(1006,$CurrentINF->getIndex($Section, $count),"ConfigPriority");
                }
            }

            if (uc($Directive) eq "DMACONFIG") {
                if ($Value =~ /:/) {
                    ($Attribs,$Num)=split(/:/,$Value);
                    if ($Attribs !~ /^[D|W|M|N]/) {
                        AddErrorRef(1066,$CurrentINF->getIndex($Section, $count),"DMAattrs", $Section);
                    }
                } else {
                    $Num = $Value;
                }
            }

            if (uc($Directive) eq "IOCONFIG") {
                unless(uc($Value) =~ /^[0-9A-F]{3}-[0-9A-F]{3}/) {
                        AddErrorRef(1066, $CurrentINF->getIndex($Section, $count),
                            "io-range", "IOConfig");
                }
            }

            if (uc($Directive) eq "IRQCONFIG") {
                if ($Value =~ /:/) {
                    ($Attribs,$Num)=split(/:/,$Value);
                    if ($Attribs !~ /^[L|LS]$/) {
                        AddErrorRef(1066,$CurrentINF->getIndex($Section, $count),"IRQConfig",$Section);
                    }
                } else {
                    $Num = $Value;
                }
            }
            $ConfigPriority++ if (uc($Directive) eq "CONFIGPRIORITY");
            $count++;
        }
        AddErrorRef(1058, $SecStart, "ConfigPriority", $Section)
            if (! $ConfigPriority);

        $CheckedSections{uc($Section)} = TRUE;
    }

    return(TRUE);
}

#-------------------------------------------------------------------------------------------------
#-- ProcInstallInterfacesSection($Section)
sub ProcInstallInterfacesSection {
    print("Starting ProcInstallInterfacesSection\n") if ($DEBUG);
    my($Section) =  $_[0];
        # HKR = HKLM,System\CurrentControlSet\Control\DeviceClasses\{InterfaceClassGuid}
    my(@lines)    = $CurrentINF->getSection($Section);
    my($SecStart) = $CurrentINF->getIndex($Section,0)-1;
    my($line);
    my($count)=0;

    my($Directive, $Value);
    my($AddInterface) = 0;
    my(%InstallInterfacesDirective)=("ADDINTERFACE",1,
                                     "INCLUDE",     1,
                                     "NEEDS",       1);


   if (defined $CheckedSections{uc($Section)} )  {
        return(FALSE);
    } else {
        foreach $line (@lines) {

            $count++, next if ($line =~ /^[\s\t]*$/);# Skip if it's a blank line

            ($Directive, $Value) = SplitLine($Section,$count,$line);

            unless (defined($InstallInterfacesDirective{uc($Directive)}))  {

                if ($Module eq "GENERIC") {
                    AddWarnRef(2202, $CurrentINF->getIndex($Section, $count),
                        "$Directive (may be device class specific)");
                } else {
                    AddWarnRef(2202, $CurrentINF->getIndex($Section, $count), $Directive)
                        unless ($Module->DCDirective($Directive));
                }
                $count++;
                next;
            }

            &{uc($Directive)}($Section, $count, $Value);
            $AddInterface++ if (uc($Directive) eq "ADDINTERFACE");
            $count++;
        }
        AddErrorRef(1058, $CurrentINF->getIndex($Section, $count)-1, "AddInterface",$Section)
            unless ($AddInterface);

        $CheckedSections{uc($Section)} = TRUE;
    }

    return(TRUE);
}

#-------------------------------------------------------------------------------------------------
#-- ProcInstallOverrideSection($Section)
sub ProcInstallOverrideSection {
    print("Starting ProcInstallOverrideSection\n") if ($DEBUG);
    my($Section) =  $_[0];
    my(@lines)    = $CurrentINF->getSection($Section);
    my($SecStart) = $CurrentINF->getIndex($Section,0)-1;
    my($line);
    my($count)=0;

    my($Directive, $Value);
    my(%InstallOverrideDirective) = ("LOGCONFIG",  1);

    if (defined $CheckedSections{uc($Section)} )  {
        return(FALSE);
    } else {

        foreach $line (@lines) {

            $count++, next if ($line =~ /^[\s\t]*$/);# Skip if it's a blank line

            ($Directive, $Value) = SplitLine($Section,$count,$line);

            unless (defined($InstallOverrideDirective{uc($Directive)}))  {
                AddWarnRef(2202, $CurrentINF->getIndex($Section,$count), $Directive)
                    unless ($Module->DCDirective($Directive));
                $count++;
                next;
            }
            &{uc($Directive)}($Section, $count, $Value);
            $count++;
        }

        $CheckedSections{uc($Section)} = TRUE;
    }

    return(TRUE);
}

#-------------------------------------------------------------------------------------------------
#-- ProcInstallSection($Section, $pnpID)
sub ProcInstallSection {
        # HKR = HKLM,System\CurrentControlSet\Control\Class\{SetupClassGUID}\{DevInstanceID}
    print("Starting ProcInstallSection\n") if ($DEBUG);
    my($Section) =  $_[0];
    my $pnpID = $_[1];
    my(@lines)    = $CurrentINF->getSection($Section);
    my($SecStart) = $CurrentINF->getIndex($Section,0)-1;
    my($line);
    my($count)=0;
    my $nonblank=0;
    my($Directive, $Value);
    my($AddReg, $CopyFiles)  =  (0,0);

    my(%InstallDirective) = ("ADDREG",                              1,
                             "BITREG",                              1,
                             "COPYFILES",                           1,
                             "COPYINF",                             1,
                             "DELFILES",                            1,
                             "DELREG",                              1,
                             "DEVICEUPGRADEUNSUPPORTED",            1,
                             "DRIVERVER",                           1,
                             "EXCLUDEID",                           1,
                             "INCLUDE",                             1,
                             "INI2REG",                             1,
                             "LOGCONFIG",                           1,
                             "NEEDS",                               1,
                             "PROFILEITEMS",                        1,
                             "REBOOT",                              1,
                             "REGISTERDLLS",                        1,
                             "RENFILES",                            1,
                             "RESTART",                             1,
                             "UNREGISTERDLLS",                      1,
                             "UPDATEINIS",                          1,
                             "UPDATEINIFIELDS",                     1,
                             "WIASECTION",                          1,
                             "FEATURESCORE",                        1,
                             "*IFTYPE",                             1,  
                             "*MEDIATYPE",                          1, 
                             "*PHYSICALMEDIATYPE",                  1,
                             );

    # If this section has already been processed, return immediately.
    if (defined $CheckedSections{uc($Section)})  {
        return(FALSE);
    }
    
    if ($INTERNAL) {
        $InstallDirective{"SYSSETUPPNPFLAGS"} = TRUE;
        $InstallDirective{"DEVICEUPGRADEUNSUPPORTED"} = TRUE;
    }

    if ($OSContext eq "Win9x") {
        $InstallDirective{"ALSOINSTALL"} = TRUE;
    }

    foreach $line (@lines) 
    {
        $count++, next if ($line =~ /^[\s\t]*$/);# Skip if it's a blank line
        $nonblank++;

        ($Directive, $Value) = SplitLine($Section,$count,$line);
        $Directive=uc($Directive);

        unless (defined($InstallDirective{uc($Directive)}))  {
              AddWarnRef(2202, $CurrentINF->getIndex($Section,$count), $Directive)
                unless ($Module->DCDirective($Directive));
            $count++;
            next;
        }

        if ($Directive =~ /^\*/)  { next; }	#skip the net specific directives starting with '*'        

        $AddReg++ if ($Directive eq "ADDREG");
        $CopyFiles++ if ($Directive eq "COPYFILES");

        if (uc($Directive) eq "ALSOINSTALL") {
            push(@lines,&{uc($Directive)}($Section, $count, $Value));
        } else {
                        $Globals{CONTEXT} = "DDINSTALL";
            &{uc($Directive)}($Section, $count, $Value, $pnpID)
                unless (($Directive eq "RESTART") or ($Directive eq "REBOOT"));
                        undef $Globals{CONTEXT};
        }

        $count++;
    }

    AddWarnRef(2211, $SecStart, "\"AddReg\" or \"CopyFiles\"", $Section)
        unless ($CopyFiles||$AddReg);

    # .HW must be checked before .Services
    if ( $CurrentINF->sectionDefined( $Section.".HW" ) ) {
        ProcInstallHWSection($Section . ".HW");
    }

    # .HW must be checked before .Services
    if ( $CurrentINF->sectionDefined( $Section.".Services" ) ) {
        ProcInstallServicesSection($Section . ".SERVICES");
    }

    if ( $CurrentINF->sectionDefined( $Section.".Interfaces" ) ) {
        ProcInstallInterfacesSection($Section . ".INTERFACES");
    }

    if ( $CurrentINF->sectionDefined( $Section.".CoInstallers" ) ) {
        ProcInstallCoInstallersSection($Section . ".COINSTALLERS");
    }

    if ( $CurrentINF->sectionDefined( $Section.".FactDef" ) ) {
        ProcInstallFactDefSection($Section . ".FACTDEF");
    }

    if ( $CurrentINF->sectionDefined( $Section.".LogConfigOverride" ) ) {
        ProcInstallOverrideSection($Section . ".LOGCONFIGOVERRIDE");
    }

    if ( $CurrentINF->sectionDefined( $Section.".NoResDup" ) ) {
        # Mark it valid, but don't check it.
        $CheckedSections{uc("$Section.NORESDUP")} = 1;
    }

    if ( $CurrentINF->sectionDefined( $Section.".PosDup" ) ) {
        # Mark it valid, but don't check it.
        $CheckedSections{uc("$Section.POSDUP")} = 1;
    }

    if ( $CurrentINF->sectionDefined( $Section.".WMI" ) ) {
        ProcWMISecuritySection("$Section".".WMI");
    }

    if ( $CurrentINF->sectionDefined( $Section. ".Wdf")) {
        ProcWdfSection($Section);
    }

    if (! $nonblank) {
        AddWarnRef(2191,$CurrentINF->getIndex($Section) -1);
    }

    $CheckedSections{uc($Section)} = TRUE;

    return(TRUE);
}

#-------------------------------------------------------------------------------------------------
#-- ProcInstallServicesSection($Section)
sub ProcInstallServicesSection {
    print("Starting ProcInstallServicesSection\n") if ($DEBUG);
    my($line);
    my($count)=0;
    my($Section) =  $_[0];
    my(@lines)   =  $CurrentINF->getSection($Section);
    my($Directive, $Value);
    my($AddService) = 0;
    my($SPSVCINST_ASSOCSERVICE)=0;

    my(@Values);
    my(%InstallServicesDirective) = ("ADDSERVICE",  1,
                                     "DELSERVICE",  1,
                                     "INCLUDE",     1,
                                     "ADDREG",      1,
                                     "NEEDS",       1);

    
    foreach $line (@lines) {
        $count++, next if ($line =~ /^[\s\t]*$/);# Skip if it's a blank line

        ($Directive, $Value) = SplitLine($Section,$count,$line);

        unless (defined($InstallServicesDirective{uc($Directive)}))  {
            AddWarnRef(2202, $CurrentINF->getIndex($Section,$count), $Directive)
                unless ($Module->DCDirective($Directive));
            $count++;
            next;
        }

        # Check for multiple services setting SPSVCINST_ASSOCSERVICE (0x2)
        if (uc($Directive) eq "ADDSERVICE") {
            @Values=GetList($Value,$CurrentINF->getIndex($Section,$count));
            $AddService++;

            if (defined($Values[1])) {
                # Handle the blank case
                if ($Values[1] =~ /^[\s\t]*$/) {
                    $Values[1]=0;
                }

                if ($Values[1]=~/^\d*$/) {
                    # Force numeric strings to numeric context
                    $Values[1]+=0;
                }

                if ($Values[1] =~ /^0x/i) {
                    $Values[1] =~ s/^0x//i;
                    $Values[1] = hex($Values[1]);
                } elsif ($Values[1] =~ /^0/) {
                    $Values[1] =~ s/^0//i;
                    $Values[1] = oct($Values[1]);
                } # Otherwise, assume it's already in decimal

                if (! is_numeric($Values[1]) ) {
                    $Values[1]=0;
                }

                $SPSVCINST_ASSOCSERVICE++ if (($Values[1] & 0x2) == 0x2);
            }
        }

        # fix for bug 508497 : assume that if a needs directive is found, the
        # corresponding section in the included inf will add atleast 1 service
        elsif (uc($Directive) eq "NEEDS") {
           $AddService++;
        }

        $Globals{CONTEXT} = "DDINSTALL.SERVICES";

	 # This calls a function according with the name of the Directive        
        &{uc($Directive)}($Section,$count, $Value);

        undef $Globals{CONTEXT};
        $count++;
    }

    AddErrorRef(1058, $CurrentINF->getIndex($Section,0)-1, "AddService", $Section) if (! $AddService);
    AddErrorRef(1011, $CurrentINF->getIndex($Section,0)-1) if ($SPSVCINST_ASSOCSERVICE>1);
    $CheckedSections{uc($Section)} = TRUE;

    return(TRUE);
}

#-------------------------------------------------------------------------------------------------
#-- ProcUninstallServicesSection($Section)
sub ProcUninstallServicesSection {
    print("Starting ProcUninstallServicesSection\n") if ($DEBUG);
    my($line);
    my($count)=0;
    my($Section) =  $_[0];
    my(@lines)   =  $CurrentINF->getSection($Section);
    my($Directive, $Value);

    my(@Values);
    my(%InstallServicesDirective) = ("DELSERVICE",  1,
                                     "INCLUDE",     1,
                                     "NEEDS",       1);

    foreach $line (@lines) {
        $count++, next if ($line =~ /^[\s\t]*$/);# Skip if it's a blank line

        ($Directive, $Value) = SplitLine($Section,$count,$line);

        unless (defined($InstallServicesDirective{uc($Directive)}))  {
            AddWarnRef(2202, $CurrentINF->getIndex($Section,$count), $Directive)
                unless ($Module->DCDirective($Directive));
            $count++;
            next;
        }

        &{uc($Directive)}($Section,$count, $Value);
        $count++;
    }
    $CheckedSections{uc($Section)} = TRUE;

    return(TRUE);
}

#-------------------------------------------------------------------------------------------------#
#-- ProcInterfaceInstall32Section($line, $Section) ---------------------------------------------#
sub ProcInterfaceInstall32Section {
    my $Section  =  shift;
    my @lines    =  $CurrentINF->getSection($Section);
    my($count)   =     0;
    my($line);
        # HKR = HKLM,System\CurrentControlSet\Control\DeviceClasses\{InterfaceClassID}
    my($Directive, $Value, @Values);

    if (defined $CheckedSections{$Section})  {
        return(FALSE);
    } else {
        $CheckedSections{$Section} = TRUE;
        print(STDERR "\tBeginning ProcInterfaceInstall32Section (@_)...\n") if ($DEBUG);

        foreach $line (@lines) {

            $count++, next if ($line =~ /^[\s\t]*$/);# Skip if it's a blank line

            ($Directive, $Value) = SplitLine($Section,$count,$line);
            @Values     = GetList($Value,$CurrentINF->getIndex($Section,$count));

            if (defined($Values[0]))  {
                ProcInterfaceInstallSection($Values[0]);
            } else {
                AddErrorRef(1025, $line, "interface-install-section");
                $count++;
                next;
            }

            if (defined($Values[1]) && ($Values[1] ne 0)) {
                AddErrorRef(1068,$line,"flags. (Expected null or 0)");
            }

            $count++;
        }
        $CheckedSections{$Section} = TRUE;
    }
    print(STDERR "\tExiting ProcInterfaceInstall32Section...\n") if ($DEBUG);
    return(TRUE);
}

#-------------------------------------------------------------------------------------------------#
#-- ProcInterfaceInstallSection($line, $Section) -------------------------------------------------#
sub ProcInterfaceInstallSection {
    my $Section  =  shift;
    my @lines    =  $CurrentINF->getSection($Section);
    my($count)   =     0;
    my($line);

    my($Directive, $Value, @Values);

    my($MIN)     =  FALSE;

    #Valid InterfaceInstall Directives
    my(%InterfaceInstallDirective)=("ADDREG",             1,
                                    "DELREG",             1,
                                    "BITREG",             1,
                                    "COPYFILES",          1,
                                    "DELFILES",           1,
                                    "RENFILES",           1,
                                    "UPDATEINIS",         1,
                                    "INI2REG",            1,
                                    "UPDATEINIFIELDS",    1);

    if (defined $CheckedSections{$Section})  {
        return(FALSE);
    } else {
        print(STDERR "\tBeginning ProcInterfaceInstallSection (@_)...\n") if ($DEBUG);
        foreach $line (@lines) {

            $count++, next if ($line =~ /^[\s\t]*$/);# Skip if it's a blank line

            ($Directive, $Value) = SplitLine($Section,$count,$line);

            unless (defined($InterfaceInstallDirective{uc($Directive)}))  {
                AddWarnRef(2202, $line, $Directive) unless ($Module->DCDirective($Directive));
                $count++;
                next;
            }

            if ((uc($Directive) eq "ADDREG") && defined($Value)){ # Must have at least one AddReg!
                $MIN = TRUE;
            }

            &{uc($Directive)}($Section, $count, $Value);

            $count++;
        }

        unless ($MIN) {
            AddErrorRef(1058,$CurrentINF->getIndex($Section)-1,"AddReg",$Section);
        }

        $CheckedSections{$Section} = TRUE;
    }
    print(STDERR "\tExiting ProcInterfaceInstallSection...\n") if ($DEBUG);
    return(TRUE);
}

#-------------------------------------------------------------------------------------------------
#-- ProcModelSection($Section)
sub ProcModelSection {
    print("Starting ProcModelSection\n") if ($DEBUG);
    my($Section) =  $_[0];	#Name of the model section
    my($count)   =     0;
    my($line);
    my(@lines)   = $CurrentINF->getSection($Section);

    my($Directive, $Value, @Values);
    my($Destination, $Source, $Temp, $Flag);
    my($ExcludeID, $ExcludeRef);
    my $pnpID;

    $Globals{"ModelSection"} = uc $Section;
    foreach $line (@lines) 
    {

        $count++, next if ($line =~ /^[\s\t]*$/);# Skip if it's a blank line

        ($Directive, $Value) = SplitLine($Section,$count,$line);
        @Values     = GetList($Value,$CurrentINF->getIndex($Section,$count));

        if (defined($Values[0])) 
        {
            $pnpID = $Values[1];
            # Check for PnPIds against WDF Samples            
            CheckWDKPnPId($pnpID);
            
            if (uc $Values[0] eq uc "DefaultInstall") 
            {
                AddWarnRef(2210, $CurrentINF->getIndex($Section,$count));
            }

            if ( $CurrentINF->sectionDefined( $Values[0] ) ) 
            {
                if (( $CurrentINF->sectionDefined( $Values[0].".NT" ) )    or
                    ( $CurrentINF->sectionDefined( $Values[0].".NTX86" ) ) or
                    ( $CurrentINF->sectionDefined( $Values[0].".NTIA64" ) ) or
                    ( $CurrentINF->sectionDefined( $Values[0].".NTAMD64" ) )) 
                {
                    $OSContext = "Win9x";
                    AddWarnRef(2208,$CurrentINF->getIndex($Values[0]) - 1);
                } 
                else 
                {
                    $OSContext = "All";
                    $Globals{"Platform"}  = "ALL";
                    # Only verify un-decorated in no NT/2000 specific sections are defined
                    ProcInstallSection( $Values[0], $pnpID );
                }
            }

            ## Per-Model reporting starts initializing here

            # Verify sections strictly for NT
            if ( $CurrentINF->sectionDefined( $Values[0].".NT") ) 
            {
                $OSContext = "Windows 2000";
                $Globals{"Platform"}  = "ALL";
                ProcInstallSection( $Values[0].".NT", $pnpID );
            }

            # Verify explicitly for NT on x86
            if ( $CurrentINF->sectionDefined( $Values[0].".NTX86") ) 
            {
                $OSContext = "Windows 2000";
                $Globals{"Platform"}  = "X86";
                ProcInstallSection( $Values[0].".NTX86", $pnpID );
            }

            # Verify explicitly for NT on IA64
            if ( $CurrentINF->sectionDefined( $Values[0].".NTIA64" ) ) 
            {
                $OSContext = "Windows 2000";
                $Globals{"Platform"} = "IA64";
                ProcInstallSection( $Values[0].".NTIA64", $pnpID );
            }

            # Verify explicitly for NT on AMD64
            if ( $CurrentINF->sectionDefined( $Values[0].".NTAMD64" ) ) 
            {
                $OSContext = "Windows 2000";
                $Globals{"Platform"} = "AMD64";
                ProcInstallSection( $Values[0].".NTAMD64", $pnpID );
            }

            # If no valid install sections are defined, error
            if ((! $CurrentINF->sectionDefined( $Values[0] ) )          and
                (! $CurrentINF->sectionDefined( $Values[0].".NT" ) )    and
                (! $CurrentINF->sectionDefined( $Values[0].".NTX86" ) ) and
                (! $CurrentINF->sectionDefined( $Values[0].".NTIA64" ) ) and
                (! $CurrentINF->sectionDefined( $Values[0].".NTAMD64" ) )) 
            {
                 AddErrorRef(1003, $CurrentINF->getIndex($Section,$count), $Values[0]);
            }

        } 
        else 
        {
            AddErrorRef(1025, $CurrentINF->getIndex($Section,$count), "install-section-name");
        }
 
        ## Need to validate PNP-ID
        if (!defined($Values[1])) {
            AddErrorRef(1025, $CurrentINF->getIndex($Section,$count), "hw-id");
        }

        do {
            AddPnpIdToList($Values[1])
                unless ($Values[1] =~ /^[\s\t]*$/);
        } while (defined shift(@Values));

        
        $count++;
    }

    $CheckedSections{uc($Section)} = TRUE;

    $Globals{"ModelSection"} = "";
    return(TRUE);
}

#-------------------------------------------------------------------------------------------------#
# Verify that (WUDFCoInstaller.dll -WudfUpdate_\d+.dll) or (WdfCoinstaller\d+.dll,WdfCoinstaller) is listed as one of the co-installers     #
# Using the WDF Co-installer is documented at http://msdn.microsoft.com/en-us/library/aa490024.aspx and at http://msdn.microsoft.com/en-us/library/aa510965.aspx
#-- ProcWdfCoinstallers($Section, $KmdfDriver, $UmdfDriver, $Win2K) ------------------------------------------------------------------#
sub ProcWdfCoinstallers {
    my $line;
    my $count = 0;
    my $Section = shift;
    my $KmdfDriver = shift;
    my $UmdfDriver = shift;    
    my $Win2K = shift;    
    my $isModuleWpd = 0;
    my($directive, $value);
    
    my ($wdfVersion, $wudfVersion);
    my ($wdfMaxVersion, $wudfMaxVersion);    
       
    unless ($CurrentINF->sectionDefined($Section)) {
        return;
    }

    if (uc($Module) eq "WPD") {
        $isModuleWpd = 1;
    }    
    
    # If this section hasn't already been checked call for the general verification
    # TEST WITH ccdcmbm INF    
    if (!defined $CheckedSections{uc($Section)}) {
        ProcInstallCoInstallersSection($Section);
    }    
        
    # RequiredRegDirectives is a map of the registry entry with values for the 
    # required reg root, reg subroot, type, found flag
    my (%requiredRegDirectives) = ('COINSTALLERS32' => ["HKR", "", "0x10000", 0],
                                   );

    # WdfCoinstaller has the CoinstallerName, CoinstallerEntryPoint, found flag entries                                       
    my (@wdfCoinstaller) = ("\^WDFCOINSTALLER(\\d+)\.DLL\$","\^WDFCOINSTALLER\$", 0);

    # WudfCoinstaller has the WudfCoinstallerName and found flag entries    
    my (@wudfCoinstaller) = ("\^WUDF(COINSTALLER|UPDATE_(\\d+))\.DLL\$", 0);
 
    $wdfVersion = 0;
    $wudfVersion = 0;                            
    my @lines = $CurrentINF->getSection($Section);
    my $addRegSecStart = 0;
    my $coinstallerLine = 0;
    $wdfMaxVersion = "0.0";
    $wudfMaxVersion = "0.0";
    foreach $line (@lines) {
        $count++, next if ($line =~ /^(\s*|;.*)$/);# Skip if it's a blank line
        ($directive,$value) = SplitLine($Section,$count,$line);

        if (uc $directive eq "ADDREG") {
            my @regSections = split /,/,$value;
            my $regSection;

            foreach $regSection (@regSections) {
                $regSection =~ s/^\s*//i; # Kill leading spaces
                $regSection =~ s/\s*$//i; # Kill trailing spaces

                if ( $CurrentINF->sectionDefined($regSection) ) {
                    my @regSectionLines = $CurrentINF->getSection($regSection);
                    my $regcount = 0;
                    my $regline;
                    $addRegSecStart = $CurrentINF->getIndex($regSection, 0) -1;                     

                    foreach $regline (@regSectionLines) {
                        $regcount++, next if ($regline =~ /^(\s*|;.*)$/);# Skip if it's a blank line
                        
                        my @regparts = split /,/,$regline;
                        
                        $regcount++, next if ($regparts[2] =~ /^(\s*|;.*)$/);# Skip if it's a blank entry
                        
                        $regparts[2]    =~  s/^\"(.*)\"/$1/;   # And Strip the Quotes!

                        if (defined($requiredRegDirectives{uc $regparts[2]})) {
                            # If we've already found this directive before, flag it as a warning
                            AddWarnRef(2301, $CurrentINF->getIndex($regSection, $regcount), $regparts[2])
                                    if ($requiredRegDirectives{uc $regparts[2]}[3] == 1 && !$isModuleWpd);
                            # Get the linenum for addreg Coinstallers32 registry key
                            $coinstallerLine = $CurrentINF->getIndex($regSection, $regcount);
                            
                            # Mark that we found the directive
                            $requiredRegDirectives{uc $regparts[2]}[3] = 1;

                            # Check that root matches
                            AddErrorRef(1505, $CurrentINF->getIndex($regSection, $regcount), $requiredRegDirectives{uc $regparts[2]}[0], $regparts[2]) 
                                    unless ($requiredRegDirectives{uc $regparts[2]}[0] eq $regparts[0] || $isModuleWpd);

                            # Check that the reg flags match
                            my $flagsRequired = $requiredRegDirectives{uc $regparts[2]}[2];
                            $flagsRequired = hex($flagsRequired) if ($flagsRequired =~ /^0x[0-9A-F]*$/i);

                            my $flagsInf = $regparts[3];
                            $flagsInf =~ s/^\s*//i; # Kill leading spaces
                            $flagsInf =~ s/\s*$//i; # Kill trailing spaces                                                        
                            $flagsInf = hex($flagsInf) if ($flagsInf =~ /^0x[0-9A-F]*$/i);

                            AddErrorRef(1506, $CurrentINF->getIndex($regSection, $regcount), $requiredRegDirectives{uc $regparts[2]}[2], $regparts[2])
                                    unless (($flagsRequired & $flagsInf) == $flagsInf || $isModuleWpd);

                            my $valindex = 4; # 4 is the index that the values start at
                            my $wdfEntryCheck = 0;
                            $wdfVersion = 0;
                            $wudfVersion = 0;
                            my $actualvalue;                            
                            # Search for the Wdf/Wudf Coinstallers                             
                            while ($valindex < scalar @regparts) {
                                $actualvalue = $regparts[$valindex];      
                                $actualvalue =~ s/^\s*|\s*$//g;  
                                $actualvalue =~ s/\"*//g;   # Strip the Quotes!
                               
                                if ($wdfEntryCheck) {
                                    $wdfEntryCheck = 0;
                                    if ($actualvalue =~ /$wdfCoinstaller[1]/i) {
                                        # Flag as error if WdfCoinstaller has already been added
                                        if ($wdfCoinstaller[2] == 1) {
                                            AddErrorRef(1500, $CurrentINF->getIndex($regSection, $regcount), "WDFCoinstaller");
                                        }
                                        else {                                        
                                            $wdfCoinstaller[2] = 1; # Mark WdfCoinstaller as found
                                        }                                        
                                    }
                                    else {
                                        # Flag error when WdfCoinstaller Entry is wrongly identified                                     
                                        AddErrorRef(1512, $CurrentINF->getIndex($regSection, $regcount), $actualvalue, "WdfCoinstallerNNNNN.dll, WdfCoinstaller");
                                    }
                                }
                                elsif ($actualvalue =~ /$wdfCoinstaller[0]/i) {
                                    # Flag as error if WdfCoinstaller has already been added
                                    if ($wdfCoinstaller[2] == 1) {
                                        AddErrorRef(1500, $CurrentINF->getIndex($regSection, $regcount), "WDFCoinstaller");
                                    }
                                    else {                                   
                                        $wdfVersion = $1;
                                        $wdfEntryCheck = 1;                                    
                                    }
                                }
                                elsif ($actualvalue =~ /$wudfCoinstaller[0]/i) {
                                    # Flag as error if WdfCoinstaller has already been added
                                    if ($wudfCoinstaller[1] == 1) {
                                        AddErrorRef(1500, $CurrentINF->getIndex($regSection, $regcount), "WUDFCoinstaller");
                                    }
                                    else {                                   
                                        $wudfCoinstaller[1] = 1;       # Mark WudfCoinstaller as found                                 
                                        if (defined $2) {
                                            $wudfVersion = $2;    # This corresponds to the second paranthesis
                                        }                
                                    }
                                }

                                $valindex++;
                            }
                            if ($KmdfDriver && $wdfEntryCheck) {
                                # At exit wdfEntryCheck must be returned back to 0                            
                                AddErrorRef(1512, $CurrentINF->getIndex($regSection, $regcount), $actualvalue, "WdfCoinstallerNNNNN.dll, WdfCoinstaller");
                            }                            
                        }

                        $regcount++;
                    }
                }
            }  
        }
        elsif (uc $directive eq "COPYFILES") {
            if ($CurrentINF->sectionDefined($value)) {        
                ($wdfMaxVersion, $wudfMaxVersion) = ProcWdfCoinstallerCopyFiles($value);
            }
        }

        $count++;
    }

    my $requiredDirective;
    foreach $requiredDirective (keys(%requiredRegDirectives)) {
        if ($requiredRegDirectives{$requiredDirective}[3] != 1) {
            AddWarnRef(2302, $addRegSecStart, $Section); 
            return;            
        }
    }

    # Check if for the UMDF driver we have at least the request for WudfCoinstaller 
    if ($UmdfDriver) {
        AddErrorRef(1516, $addRegSecStart)
             unless ($wudfCoinstaller[1] == 1);
    }

    # Check if for the KMDF driver we have any co-installer request
    # If wdf co-installer is not provided flag an error for Win2K
    # If the platform is XP-above then flag a warning instead    
    if ($KmdfDriver && $wdfCoinstaller[2] != 1) {
        if ($Win2K) {    
            AddWarnRef(2304, $addRegSecStart);
        }
        else {        
            AddWarnRef(2303, $addRegSecStart);
        }            
    }
    
    # Check whether the max versions of kmdf coinstallers have been added to the registry
    if ($wdfMaxVersion ne "0.0") {
        # WdfCoinstaller hasn't even added for Coinstallers32 registry key    
        if ($wdfCoinstaller[2] != 1) { 
            AddErrorRef(1513, $addRegSecStart, "WdfCoinstaller", $wdfMaxVersion);
        }
        elsif ($wdfVersion =~ /^(\d{2})(\d{3})$/) {                
            my ($wdfMajor, $wdfMinor);
            $wdfMajor = $1 + 0;
            $wdfMinor = $2 + 0;
            if ($wdfMaxVersion !~ /^$wdfMajor\.$wdfMinor$/){
                AddErrorRef(1513, $coinstallerLine, "WdfCoinstaller", $wdfMaxVersion);
            } 
        }
        else {
            AddErrorRef(1513, $coinstallerLine, "WdfCoinstaller", $wdfMaxVersion);
        }
    }

    # Check whether the max versions of wudf coinstallers have been added to the registry
    if ($wudfMaxVersion ne "0.0") {
        # WudfCoinstaller hasn't even added for Coinstallers32 registry key    
        if ($wudfCoinstaller[1] != 1) {
            AddErrorRef(1513, $addRegSecStart, "WudfCoinstaller", $wudfMaxVersion);
        }
        elsif ($wudfVersion =~ /^(\d{2})(\d{3})$/) {                
            my ($wudfMajor, $wudfMinor);
            $wudfMajor = $1 + 0;
            $wudfMinor = $2 + 0;
            if ($wudfMaxVersion !~ /^$wudfMajor\.$wudfMinor$/){
                AddErrorRef(1513, $coinstallerLine, "WudfCoinstaller", $wudfMaxVersion);
            } 
        }
        else {
            AddErrorRef(1513, $coinstallerLine, "WudfCoinstaller", $wudfMaxVersion);
        }
    }
}

#----------------------------------------------------------------------------#
# Find the latest version of the wdf and wudf coinstallers added through Copyfiles
#-- ProcWdfCoinstallerCopyFiles($Section) ------------------------------------------------------------------#
sub ProcWdfCoinstallerCopyFiles {
    print("Starting ProcWdfCoInstallersCopyFiles\n") if ($DEBUG);
    my($Section) =  $_[0];
    
    unless ($CurrentINF->sectionDefined($Section)) {
        return ("0.0", "0.0");;
    }        
    
    my(@lines)    = $CurrentINF->getSection($Section);
    my($line);
    my($count)=0;
    
    my ($wdfMaxMajor, $wdfMaxMinor);
    my ($wudfMaxMajor, $wudfMaxMinor);    
    my ($version, $major, $minor);
    my ($wdfCoinstaller, $wudfCoinstaller);    
        
    $wdfMaxMajor = 0;
    $wdfMaxMinor = 0;    
    $wudfMaxMajor = 0;
    $wudfMaxMinor = 0;    
    foreach $line (@lines) {
        $count++, next if ($line =~ /^(\s*|;.*)$/);# Skip if it's a blank line
     
        my @values = GetList($line,$CurrentINF->getIndex($Section,$count));
        my $destination = shift(@values); 
        $wdfCoinstaller = 0;
        $wudfCoinstaller = 0;
        if ((defined ($destination)) && ($destination !~ /^\s*$/) ) {
            #Check for Wdf Coinstaller
            if ($destination =~ /^WDFCOINSTALLER(\d+)\.DLL$/i) {
                $wdfCoinstaller =1;
            }
            elsif ($destination =~ /^WUDFUPDATE_(\d+)\.DLL$/i) { 
                # Otherwise check for Wudf Coinstaller            
                $wudfCoinstaller = 1;
            }

            if ($wdfCoinstaller == 1 || $wudfCoinstaller == 1) {
                $version = $1;
                if ($version =~ /^(\d{2})(\d{3})$/) {
                    $major = $1;
                    $minor = $2;

                    if ($wdfCoinstaller) {
                        if (scalar $major > $wdfMaxMajor) {
                            $wdfMaxMajor = $major + 0;
                            $wdfMaxMinor = $minor + 0;                        
                        }
                        elsif (scalar $minor > $wdfMaxMinor) {
                            $wdfMaxMinor = $minor + 0;
                        }
                    }
                    elsif ($wudfCoinstaller) {
                        if (scalar $major > $wudfMaxMajor) {
                            $wudfMaxMajor = $major + 0;
                            $wudfMaxMinor = $minor + 0;                        
                        }
                        elsif (scalar $minor > $wudfMaxMinor) {
                            $wudfMaxMinor = $minor + 0;
                        }                    
                    }                    
                }  
                else {
                    AddErrorRef(1514, $CurrentINF->getIndex($Section,$count),"WdfCoinstaller", "WdfCoinstallerMMNNN.dll")
                        if ($wdfCoinstaller == 1);

                    AddErrorRef(1514, $CurrentINF->getIndex($Section,$count), "WudfCoinstaller", "WudfUpdateMMNNN.dll")
                        if ($wudfCoinstaller == 1);                       
                }
            }
        }
        $count++;
    }

    return ($wdfMaxMajor.".".$wdfMaxMinor, $wudfMaxMajor.".".$wudfMaxMinor);
}

#---------------------------------------------------------------------------------------------------------------#
# $ControlFlag is used to specify which services need to be confirmed
# If bit 0 is set validate installation sections for WudfRd service of a UMDF driver
# If bit 1 is set validate installation sections for WinUsb service of a UMDF driver
#-- CheckServiceInstall($Section, $Service) ---------------------------------------------------------------------------#
sub CheckServiceInstall {
    my $Section = shift;
    my $ControlFlag = shift;
    my $ResultFlag = 0;    
    my $serviceBinary;
  
    my $checkWudfRd = 0;
    my $wudfRdControl = 1;

    my $checkWinUsb = 0;
    my $winUsbControl =2;

    if (($ControlFlag & $wudfRdControl) == $wudfRdControl) {
        $checkWudfRd = 1;
        $serviceBinary = "wudfrd.sys";
    }
    elsif (($ControlFlag & $winUsbControl) == $winUsbControl) {
        $checkWinUsb = 1;
        $serviceBinary = "winusb.sys";        
    }
    else {
        return $ResultFlag;
    }        

    my $expectedServiceType = 1;
    my $expectedStartType = 3;
    my $expectedErrorControl = 1;
    my $serviceMatch = 0;
    my ($serviceType, $serviceTypeLine);
    my ($startType, $startTypeLine);
    my ($errorControl, $errorControlLine);               
    
    if ($CurrentINF->sectionDefined($Section)) {

        my($directive, $value);   
        my @lines = $CurrentINF->getSection($Section);
        my $count = 0;
        my $line;
     
        foreach $line (@lines) {
            $count++, next if ($line =~ /^(\s*|;.*)$/);# Skip if it's a blank line
                        
            ($directive,$value) = SplitLine($Section,$count,$line);

            if (uc $directive eq 'SERVICEBINARY') {
                # Check that WUDFrd.sys is being used as the service binary
                $value    =~  s/^\"(.*)\"/$1/;   # Strip the Quotes!
                if ($value =~ /^\$WinDir\$\\system32\\drivers\\($serviceBinary)$/i) {
                   $serviceMatch = 1;
                }
                else {
                    last;
                }                
            }
            elsif (uc $directive eq 'SERVICETYPE') {
                $serviceType = $value;
                if ($serviceType =~ /^0X/i) {                
                    $serviceType =~ s/^0X//i;
                    $serviceType = hex($serviceType);                    
                }
                $serviceTypeLine = $CurrentINF->getIndex($Section, $count);                
            }
            elsif (uc $directive eq 'STARTTYPE') {
                $startType = $value;
                if ($startType =~ /^0X/i) {                
                    $startType =~ s/^0X//i;
                    $startType = hex($startType);                    
                }               
                $startTypeLine = $CurrentINF->getIndex($Section, $count);                
            }
            elsif (uc $directive eq 'ERRORCONTROL') {
                $errorControl = $value;
                if ($errorControl =~ /^0X/i) {                
                    $errorControl =~ s/^0X//i;
                    $errorControl = hex($errorControl );                    
                }            
                $errorControlLine = $CurrentINF->getIndex($Section, $count);                
            }
            $count++;
        }

        # If this section hasn't been processed, mark as visited.
        if (!defined $CheckedSections{uc($Section)})  {
            $CheckedSections{uc($Section)} = TRUE;            
        }    

    }
    else {
        #No need to add an error since this will be observed by ProcServices
    }

    if ($serviceMatch) {
        $ResultFlag = $ControlFlag;

        AddErrorRef(1502, $serviceTypeLine , "ServiceType", $expectedServiceType)
            unless ($serviceType == $expectedServiceType);

        AddErrorRef(1502, $startTypeLine, "StartType", $expectedStartType)
            unless ($startType == $expectedStartType);

        AddErrorRef(1502, $errorControlLine, "ErrorControl", $expectedErrorControl)
                    unless ($errorControl == $expectedErrorControl);

        return $ResultFlag;                    
    }
}

#---------------------------------------------------------------------------------------------------#
# Checks Services section to verify Wdf related services are referenced properly
# $ControlFlag is used to specify which services need to be confirmed
# If bit 0 is set validate installation sections for WudfRd service of a UMDF driver
# If bit 1 is set validate installation sections for WinUsb service of a UMDF driver
# If bit 2 is set validate installation sections for the KMDF driver- if this bit is set %KmdfServiceNames should be provided
#-- CheckWdfSpecificServices($Section, $ControlFlag)-------------------------------------------#
sub CheckWdfSpecificServices {
    print("Starting CheckWdfSpecificServices\n") if ($DEBUG);

    my $Section = shift;
    my $ControlFlag = shift;

    my $checkWudfRd = 0;
    my $wudfRdControl = 1;

    my $checkWinUsb = 0;
    my $winUsbControl =2;

    my $checkKmdf = 0;
    my $kmdfControl = 4;
    my %KmdfServiceNames;    
    
    # INF must have a [DDInstall.Services] for each [DDInstall] section
    # If this doesn't exist it will be catched    
    unless ($CurrentINF->sectionDefined($Section)) {   
        return;
    }

    if (($ControlFlag & $wudfRdControl) == $wudfRdControl) {
        $checkWudfRd = 1;
    }
    
    if (($ControlFlag & $winUsbControl) == $winUsbControl) {
        $checkWinUsb = 1;
    }
    
    if (($ControlFlag & $kmdfControl) == $kmdfControl) {
        $checkKmdf = 1;
        %KmdfServiceNames = shift;
    }
    
    my $resultFlag = 0;
    my $svcUsesWudfRd = 0;
    my $svcUsesWinUsb = 0;
    my $svcIncludeWinUsbInf = 0;     
    my ($directive, $value);
    my @values;
    my @lines = $CurrentINF->getSection($Section);   
    my $line;
    my $count = 0;    
    foreach $line (@lines) {
        $count++, next if ($line =~ /^(\s*|;.*)$/);# Skip if it's a blank line
        
        ($directive,$value) = SplitLine($Section,$count,$line);
        
        if (uc $directive eq 'ADDSERVICE') {
            @values = GetList($value, $CurrentINF->getIndex($Section, $count));

            # AddService has a min of 3 values, unless it's the case chkinf will indicate the error
            if (scalar @values >= 3)
            {                
                # Check for WudfRd service and flags                
                if ($checkWudfRd && !$svcUsesWudfRd) {
                    $resultFlag = CheckServiceInstall($values[2], $wudfRdControl);
                    if (($resultFlag & $wudfRdControl) == $wudfRdControl) {
                        $svcUsesWudfRd  = 1;  
                        $count++, next;                                                
                    }                        
                }
                if ($checkWinUsb && !$svcUsesWinUsb && !$svcIncludeWinUsbInf) { #Check WinUsb Service and Flags
                    $resultFlag = CheckServiceInstall($values[2], $winUsbControl);
                    if (($resultFlag & $winUsbControl) == $winUsbControl) {
                        $svcUsesWinUsb = 1;
                        # Not skipping the check for KmdfServiceNames, since we might be checking for winsub for both UMDF and KMDF                        
                    }
                }
                if ($checkKmdf) {
                    my $kmdfServiceName;                    
                    foreach $kmdfServiceName (keys %KmdfServiceNames) {
                        if ($kmdfServiceName eq uc $values[0]) {
                            $KmdfServiceNames{$kmdfServiceName} = 1;
                            last;  # KmdfService should be listed just once                            
                        }
                    }
                }
            }
        }
        elsif (uc($directive) eq 'INCLUDE') {
            if (uc($value) eq 'WINUSB.INF') {
                $svcIncludeWinUsbInf = 1;
            }
        }
        $count++;        
    }#foreach line

    if ($checkWudfRd == 1 && $svcUsesWudfRd != 1) {
        AddErrorRef(1501, $CurrentINF->getIndex($Section, 0) - 1, "WUDFRd");             
    }

    if ($checkWinUsb == 1 && $svcUsesWinUsb != 1 && $svcIncludeWinUsbInf != 1) {
        AddErrorRef(1501, $CurrentINF->getIndex($Section, 0) - 1, "WinUsb");
    }

    if ($checkKmdf) {
        my $kmdfServiceName;
        foreach $kmdfServiceName (keys %KmdfServiceNames) {
            AddErrorRef(1501, $CurrentINF->getIndex($Section, 0) - 1,"Wdf Driver: ".$kmdfServiceName)
                unless($KmdfServiceNames{$kmdfServiceName} == 1);            
        }
    }
}

#-----------------------------------------------------------------------------------------------------------#
# This function returns the UMDF library version string if the section is valid
#-- ProcUMDFServiceInstall($Section) ---------------------------------------------------------------------------#
sub ProcUmdfServiceInstall {
    print("Starting ProcUmdfServiceInstall\n") if ($DEBUG);
    my $Section = shift;
    my $umdfVersion = "0.0.0";    
    my $count = 0;
    my $line;

    # If the $Module is Wpd, don't double the error output
    my $isModuleWpd = 0; 
    
    my($directive, $value);
    # If this section has already been processed, return immediately.
    if (defined $CheckedSections{uc($Section)})  {
        return $umdfVersion;
    }

    if (uc($Module) eq "WPD") {
        $isModuleWpd = 1;
    }        
    
    my (%requiredDirectives) = ('UMDFLIBRARYVERSION'    => 0,
                                'SERVICEBINARY'         => 0,
                                'DRIVERCLSID'           => 0
                                );
    my @lines = $CurrentINF->getSection($Section);                                
    foreach $line (@lines) {
        $count++, next if ($line =~ /^(\s*|;.*)$/);# Skip if it's a blank line
        
        ($directive,$value) = SplitLine($Section,$count,$line);
        
        if (defined($requiredDirectives{uc $directive})) {
            
            # If we have already found this directive, flag it as an error
            if ($requiredDirectives{uc $directive} == 1) {
                AddErrorRef(1508, $CurrentINF->getIndex($Section, $count), $directive);
                $count++, next;                
            }
            else {            
                $requiredDirectives{uc $directive} = 1;  # Mark as found
            }                
        }
       
        if (uc $directive eq 'UMDFLIBRARYVERSION') {
            my @versionValues = split /\./, $value;
            if (scalar(@versionValues) != 3) {
                AddErrorRef(1503, $CurrentINF->getIndex($Section, $count), $directive, "major.minor.service")
                    unless ($isModuleWpd);                      
            }
            else {
                $umdfVersion = $value;
            }
            
        }
        elsif (uc $directive eq 'SERVICEBINARY') {
            # Check if the service binary is in %12%\UMDF, if not issue a warning
            $value    =~  s/^\"(.*)\"/$1/;   # Strip the Quotes!
            
            AddWarnRef(2300, $CurrentINF->getIndex($Section, $count))
                unless ($value =~ /^\$WinDir\$\\system32\\drivers\\umdf\\.+$/i || $isModuleWpd);
       }
        elsif (uc $directive eq 'DRIVERCLSID') {   
            $value    =~  s/^\s*\"(.*)\"\s*/$1/;   # Strip the Quotes!
            
            # braces around the guid are optional, so add them if not present
            if ($value !~ /\A\{/ && $value !~ /\}\z/) {
                $value = "{" . $value . "}";
            }
                
            AddErrorRef(1504, $CurrentINF->getIndex($Section, $count))        
                        if (($value !~ /^\{[a-fA-F0-9]{8}-[a-fA-F0-9]{4}-[a-fA-F0-9]{4}-[a-fA-F0-9]{4}-[a-fA-F0-9]{12}\}$/ && !$isModuleWpd));

            # Check for DriverCLSID against WDF sample CLSIDs
            CheckUmdfCLSID($value, $CurrentINF->getIndex($Section, $count));                        
        }
        else {
            AddErrorRef(1507, $CurrentINF->getIndex($Section, $count), $directive, $Section);
        }        
        
        $count++;
    }    
    
    # Check that all required directives were present
    my $requiredDirective;
    foreach $requiredDirective (keys(%requiredDirectives)) {
        AddErrorRef(1058, $CurrentINF->getIndex($Section, 0)-1, $requiredDirective, $Section) 
                unless ($requiredDirectives{$requiredDirective} == 1 || $isModuleWpd);           
    }

    $CheckedSections{uc($Section)} = TRUE;

    return $umdfVersion;
}

#-----------------------------------------------------------------------------------------------------------#
#-- ProcWdFServiceInstall($Section) ---------------------------------------------------------------------------#
sub ProcKmdfServiceInstall {
    print("Starting ProcWdfSection\n") if ($DEBUG);
    my $Section = shift;
    my $count = 0;
    my $line;    

    my($directive, $value);
    # If this section has already been processed, return immediately.
    if (defined $CheckedSections{uc($Section)})  {
        return(FALSE);
    }
    
    my (%requiredDirectives) = ('KMDFLIBRARYVERSION' => 0);
    my @lines = $CurrentINF->getSection($Section);                                
    foreach $line (@lines) {
        $count++, next if ($line =~ /^(\s*|;.*)$/);# Skip if it's a blank line
        
        ($directive,$value) = SplitLine($Section,$count,$line);
        
        if (defined($requiredDirectives{uc $directive})) {
            # If we have already found this directive, flag it as an error        
            if ($requiredDirectives{uc $directive} == 1) {
                AddErrorRef(1508, $CurrentINF->getIndex($Section, $count), $directive);
                $count++, next;                
            }
            else {            
                $requiredDirectives{uc $directive} = 1;  # Mark as found
            }                
        }

        if (uc($directive) ne "KMDFLIBRARYVERSION") {
            AddErrorRef(1507, $CurrentINF->getIndex($Section, $count), $directive, $Section);
        }
        else {
            my @versionValues = split /\./, $value;
             AddErrorRef(1503, $CurrentINF->getIndex($Section, $count), $directive, "major.minor")
                unless (scalar(@versionValues) == 2);             
        }        
        $count++;        
    }

    # Check that all required directives were present
    my $requiredDirective;
    foreach $requiredDirective (keys(%requiredDirectives)) {
        AddErrorRef(1058, $CurrentINF->getIndex($Section, 0)-1, $requiredDirective, $Section) 
                unless ($requiredDirectives{$requiredDirective} == 1);           
    }

    $CheckedSections{uc($Section)} = TRUE;
}

#-------------------------------------------------------------------------------------------------#
#-- ProcWDFSection($DDInstall) ---------------------------------------------------------------------------#
sub ProcWdfSection {
    print("Starting ProcWdfSection\n") if ($DEBUG);
    my $count = 0;
    my $DDInstall = $_[0];
    my $Section = $DDInstall. ".Wdf";  
   
    my @lines = $CurrentINF->getSection($Section);

    my (%umdfRequiredDirectives) = ('UMDFSERVICE' => 0,
                                'UMDFSERVICEORDER'  => 0
                                );

    my (%kmdfRequiredDirectives) = ('KMDFSERVICE'=> 0);                                
                                
    # If the $Module is Wpd, don't double the error output in case driver has UMDF specific settings
    my $isModuleWpd = 0;                           
    my @umdfServiceOrderValues;
    my %umdfServiceNames;
    my $umdfServiceOrderLine = 0;
    my $isDispatchWinUsb = 0;

    my $isKmdfDriver = 0;
    my $isUmdfDriver = 0;    
    my %kmdfServiceNames;
 
    my $line;
    my ($directive, $value, @values);
    
    # If this section has already been processed, return immediately.
    if (defined $CheckedSections{uc($Section)})  {
        return;
    }

    if (uc($Module) eq "WPD") {
        $isModuleWpd = 1;
    }   

    # Store Umdf Versions in a hash
    my %umdfLibVersions;

    # Hold the "LineNum.Directive"-Version pairs for directives
    # that require a specific version    
    my %umdfCheckVersions;
    
    foreach $line (@lines) {
        $count++, next if ($line =~ /^(\s*|;.*)$/);# Skip if it's a blank line
        
        ($directive, $value) = SplitLine($Section, $count, $line);
        
        if (defined($umdfRequiredDirectives{uc $directive})) {
            $umdfRequiredDirectives{uc $directive} = 1;  # Mark as found
             $isUmdfDriver = 1;            
        }

        if (defined($kmdfRequiredDirectives{uc $directive})) {
            $kmdfRequiredDirectives{uc $directive} = 1;  # Mark as found
            $isKmdfDriver = 1;            
        }
        
        if (uc $directive eq 'UMDFSERVICE') {
            $value =~ s/\"*//g;
            @values = GetList($value, $CurrentINF->getIndex($Section, $count));
            
            if (scalar @values == 2)
            {  
                $umdfServiceNames{uc $values[0]} = 0;

                # Check for UmdfService against WDF sample service Names
                CheckWdfServiceName($values[0], $CurrentINF->getIndex($Section, $count), "UmdfService");
                
                if ($CurrentINF->sectionDefined($values[1])) {
                    my $umdfVersion;                
                    $umdfVersion = ProcUmdfServiceInstall($values[1]);
                    if ($umdfVersion ne "0.0.0" && !defined($umdfLibVersions{$umdfVersion})) {
                        $umdfLibVersions{$umdfVersion} = 1;
                    }                    
                } 
                elsif (!$isModuleWpd) {                      
                    AddErrorRef(1003, $CurrentINF->getIndex($Section, $count), $values[1]);
                }
            }
            elsif (!$isModuleWpd) {
                AddErrorRef(1503, $CurrentINF->getIndex($Section, $count), $directive, "ServiceName, ServiceInstallSectionName");
            }
        }
        elsif (uc $directive eq 'UMDFSERVICEORDER') {
            # Check if this has been already defined
            if ($umdfServiceOrderLine != 0) {
                # Only one UmdfServiceOrder directive must be present
                AddErrorRef(1508, $CurrentINF->getIndex($Section, $count), $directive)
                    unless ($isModuleWpd);                    
            }
            else {            
                # Post-process this once we have all service name values
                @umdfServiceOrderValues = GetList($value, $CurrentINF->getIndex($Section, $count));
                $umdfServiceOrderLine = $CurrentINF->getIndex($Section, $count);
            }
        }    
        elsif (uc $directive eq 'UMDFIMPERSONATIONLEVEL') {
            my %umdfImpersonationLevelValues = ('ANONYMOUS' => 0,
                                            'IDENTIFICATION' => 0,
                                            'IMPERSONATION' => 0,
                                            'DELEGATION' => 0,
                                            );
                                                
            AddErrorRef(1509, $CurrentINF->getIndex($Section, $count), $directive) 
                unless (defined($umdfImpersonationLevelValues{uc $value}));  
        }
        elsif (uc $directive eq 'UMDFMETHODNEITHERACTION') {
            my %umdfNeitherActionValues = ('REJECT' => 0,
                                            'COPY' =>0,
                                            );
            # WPD doesn't have check for this directive
            AddErrorRef(1509, $CurrentINF->getIndex($Section, $count), $directive) 
                unless (defined($umdfNeitherActionValues{uc $value}));                                        
        }
        elsif (uc $directive eq 'UMDFDISPATCHER') {
            my %umdfDispatcherValues = ('FILEHANDLE' => 0,
                                            'WINUSB' =>0,
                                            );
            # WPD doesn't have check for this directive
            AddErrorRef(1509, $CurrentINF->getIndex($Section, $count), $directive) 
                unless (defined($umdfDispatcherValues{uc $value}));
            if (uc($value) eq "WINUSB") {
                $isDispatchWinUsb = 1; 
            }
        }
        elsif (uc $directive eq 'UMDFKERNELMODECLIENTPOLICY') {
            my %umdfKmClientPolicyValues = ('ALLOWKERNELMODECLIENTS' => 0,
                                            'REJECTKERNELMODECLIENTS' => 0
                                            );        

            AddErrorRef(1509, $CurrentINF->getIndex($Section, $count), $directive)
                unless (defined($umdfKmClientPolicyValues{uc $value}));
            my $key =$CurrentINF->getIndex($Section, $count);                
            $key = $key."\.".$directive;           
            $umdfCheckVersions{$key} = "1.9.0";                
        }
        elsif (uc $directive eq 'KMDFSERVICE') {
            @values = GetList($value, $CurrentINF->getIndex($Section, $count));
            if (scalar @values != 2) {
                AddErrorRef(1503, $CurrentINF->getIndex($Section, $count), $directive, "DriverService, Wdf-install-section");
            }
            else {          
                $kmdfServiceNames{uc $values[0]} = 0;

                # Check for kmdfService against WDF sample service Names
                CheckWdfServiceName( $values[0], $CurrentINF->getIndex($Section, $count), "KmdfService");

                if ($CurrentINF->sectionDefined($values[1])) {
                    ProcKmdfServiceInstall($values[1]);                               
                }
                else {
                    AddErrorRef(1003, $CurrentINF->getIndex($Section, $count), $values[1]);
                }              
            }            
        }
        else {
            AddErrorRef(1507, $CurrentINF->getIndex($Section, $count), $directive, $Section);
        }
        $count++;
    }
   
    my $controlFlags = 0;
    
    if($isUmdfDriver == 1)
    {
        # If UmdfServiceOrder is not specified UmdfServiceOrderLine should specify Section beginning        
        if ($umdfServiceOrderLine == 0) {
            $umdfServiceOrderLine = $CurrentINF->getIndex($Section, 0) - 1;
        }
        
        # Check that the service order contains only the service names
        # listed in UmdfService entries
        my $umdfServiceName;
        foreach $umdfServiceName (@umdfServiceOrderValues) {
            if (defined($umdfServiceNames{uc $umdfServiceName})) {
                # Mark as found
                $umdfServiceNames{uc $umdfServiceName} = 1;
            }
            else {
                # ServiceOrder uses service name not defined using UmdfService directive
                AddErrorRef(1510, $umdfServiceOrderLine, $umdfServiceName)
                    unless ($isModuleWpd);                
            }
        }

        # Make sure that all service names are present in the ServiceOrder directive
        foreach $umdfServiceName (keys(%umdfServiceNames)) {
            AddErrorRef(1511, $umdfServiceOrderLine, $umdfServiceName)
                unless ($umdfServiceNames{$umdfServiceName} == 1 || $isModuleWpd);
        }

        # Check that all required directives were present
        my $requiredDirective;
        foreach $requiredDirective(keys(%umdfRequiredDirectives)) {
            AddErrorRef(1058, $CurrentINF->getIndex($Section, 0) - 1, $requiredDirective, $Section) 
                unless ($umdfRequiredDirectives{$requiredDirective} == 1 || $isModuleWpd);             
        }

        $controlFlags = 1; #Check for WudfRd Service        
        if ($isDispatchWinUsb == 1) {
            $controlFlags |= 2; #Check for WinUsb Service             
        }

        my $umdfNumRequiredVersions = keys %umdfCheckVersions;        
        # Check for the UMDF versions in case there are any asks for it
        if ($umdfNumRequiredVersions > 0) {
            CheckUmdfRequiredVersions(\%umdfLibVersions, \%umdfCheckVersions);
        }
    }

    # There's something wrong here since we have neither a Kmdf Driver nor a Umdf Driver
    # If there's no Umdf Directive then Kmdf Directives need to exist
    if ($isUmdfDriver == 0 && $isKmdfDriver == 0) {
        AddErrorRef(1058, $CurrentINF->getIndex($Section, 0) - 1, "KmdfService", $Section);        
    }

    if ($isKmdfDriver) {
        $controlFlags |= 4;
        CheckWdfSpecificServices($DDInstall.".Services", $controlFlags, %kmdfServiceNames);
    }
    elsif ($controlFlags){
        CheckWdfSpecificServices($DDInstall.".Services", $controlFlags);
    }    
    
    # If the current ModelSection doesn't have any platform-architecture extensions then it's for 
    # Windows 2000 platform
    my $isWin2K = 0;    
    if ($Globals{"ModelSection"} !~ /\.NT/i) {
        $isWin2K = 1;
    }    
    
    if ($CurrentINF->sectionDefined($DDInstall.".CoInstallers")) {
        ProcWdfCoinstallers($DDInstall.".CoInstallers", $isKmdfDriver, $isUmdfDriver, $isWin2K);
    }
    else {
        # Otherwise check if there's any need for Wdf or Wudf Co-installers
        if ($isUmdfDriver) {
            AddErrorRef(1516, $CurrentINF->getIndex($Section, 0) - 1);
        }

        if ($isKmdfDriver) {
            if ($isWin2K) {
                AddWarnRef(2304, $CurrentINF->getIndex($Section, 0) - 1);
            }
            else {            
                AddWarnRef(2303, $CurrentINF->getIndex($Section, 0) - 1);
            }                
        }
    }

    $CheckedSections{uc($Section)} = TRUE;
    
}

#-------------------------------------------------------------------------------------------------
# Check if all the required versions within UmdfCheckVersions are supported within at least one of the UmdfLibVersions
#-- CheckUmdfRequiredVersions(%UmdfLibVersions, %UmdfCheckVersions)
sub CheckUmdfRequiredVersions {
    my %UmdfLibVersions = %{(shift)};
    my %UmdfCheckVersions = %{(shift)};
    
    my ($requiredMajor, $requiredMinor, $requiredService);
    my ($major, $minor, $service); 

    my ($umdfLineDotDirective, $umdfVersion, $version);
    foreach $umdfLineDotDirective (keys %UmdfCheckVersions) {
        $umdfVersion = $UmdfCheckVersions{$umdfLineDotDirective};
        if ($umdfVersion =~ /(\d*)\.(\d*).(\d*)/) {
            $requiredMajor = $1 + 0; # Make them scalar
            $requiredMinor = $2 + 0; 
            $requiredService = $3 + 0;
            my $versionFound = 0; 
            foreach $version (keys %UmdfLibVersions) {
                if ($version =~  /(\d*)\.(\d*).(\d*)/) {
                    $major = $1 + 0; # Make them scalar
                    $minor = $2 + 0; 
                    $service = $3 + 0;
                    if ($major > $requiredMajor) {
                        $versionFound = 1;
                        last;
                    }                        
                    elsif ($major == $requiredMajor) {
                        if ($minor > $requiredMinor) {
                            $versionFound = 1;
                            last;
                        }
                        elsif ($minor == $requiredMinor && $service >= $requiredService) {
                            $versionFound = 1;
                            last;                            
                        }
                    }
                } # else is catched within ProcWfSection
            }

            if (!$versionFound) { 

                my ($lineNum, $directive);
                if ($umdfLineDotDirective =~ /(.*)\.(.*)/) {
                    $lineNum = $1;
                    $directive = $2;                    
                    AddErrorRef(1517, $1, $2, "UMDF", $umdfVersion);
                }

            }
        } # else is catched within ProcWdfSection
    }    

}

#-------------------------------------------------------------------------------------------------
#-- CheckPnpId($PnpId)
sub CheckWDKPnPId {
    my $PnpId = shift;     

    # Skip if the PnPId is blank
    if ($PnpId =~ /^[\s\t]*$/) {    
        return;
    }        
    
    my (%wdkPnpIds) = ("USB\\VID_0547&PID_1002"                            => 1,
            "1394\\MICROSOFT&1394_DIAGNOSTIC_DEVICE"                       => 1,
            "*WCO5933"                                                     => 1,
            "PCI\\VEN_10E8&DEV_4750"                                       => 1,
            "ROOT\\ECHO"                                                   => 1,
            "{B85B7C50-6A01-11D2-B841-00C04FAD5171}\\FAKEMODEM"            => 1,
            "*PNP0BAAD"                                                    => 1,
            "ROOT\\NDISEDGE"                                               => 1,
            "{B85B7C50-6A01-11D2-B841-00C04FAD5171}\\NDISEDGE"             => 1,
            "MS_NDISPROT"                                                  => 1,
            "{6FDE7521-1B65-48AE-B628-80BE62016026}\\OSRUSBFXRAWPDO"       => 1,
            "PCI\\VEN_8086&DEV_1229"                                       => 1,
            "PCI\\VEN_8086&DEV_103D"                                       => 1,
            "PCI\\VEN_8086&DEV_1031"                                       => 1,
            "PCI\\VEN_8086&DEV_1038"                                       => 1,
            "PCI\\VEN_10b5&DEV_5601"                                       => 1,
            "PCI\\VEN_10b5&DEV_9601"                                       => 1,  
            "RAMDISK"                                                      => 1,
            "*PNP0500"                                                     => 1,
            "*PNP0501"                                                     => 1,
            "ROOT\\DYNAMBUS"                                               => 1,
            "ROOT\\STATBUS"                                                => 1,
            "{B85B7C50-6A01-11D2-B841-00C04FAD5171}\\MSTOASTER"            => 1,
            "ROOT\\TOASTMON"                                               => 1,
            "USB\\VID_045E&PID_930A"                                       => 1,
            "ROOT\\WMISAMP"                                                => 1,
            "V1394\\1394_VIRTUAL_DEVICE"                                   => 1,
            "WUDF\\ECHO"                                                   => 1,
            "UMB\\WINDOWSSIDESHOWNODEVICE"                                 => 1,
            "USB\\VID_045E&PID_94AA&MI_00"                                 => 1,
            "UMDFSAMPLES\\SKELETON"                                        => 1,
            "WUDF\\SOCKETECHO"                                             => 1,
            "WUDF\\BASIC"                                                  => 1,
            "ROOT\\MSTOASTER"                                              => 1, 
            "SENSORS\\SENSORSKELETON"                                      => 1,    
            "SENSORS\\TIMESENSOR"                                          => 1,
            "UMDF\\VIRTUALSERIAL"                                          => 1,
            "WUDF\\WPDBASICHARDWARE"                                       => 1,
            "WUDF\\WPDHELLOWORLD"                                          => 1,
            "WUDF\\MULTITRANSPORT"                                         => 1,
            "WUDF\\WPDSERVICE"                                             => 1,             
            "BTHENUM\\{C07508F2-B970-43CA-B5DD-CC4F2391BEF4}"              => 1, 
            "ROOT\\PORTIO"                                                 => 1,
            "HID\\VID_045E&PID_001E"                                       => 1,
            "HID\\VID_045E&PID_0029"                                       => 1,
            "HID\\VID_045E&PID_0039"                                       => 1,
            "HID\\VID_045E&PID_0040"                                       => 1,
            "HID\\VID_045E&PID_0047"                                       => 1,
            "ACPI\\PNP0501"                                                => 1,
            "HID\\PNP0501&CoL01"                                           => 1,
            "ACPI\\WACF004"                                                => 1,
            "ACPI\\WACF006"                                                => 1,
            "{A65C87F9-BE02-4ED9-92EC-012D416169FA}\\KEYBOARDFILTER"       => 1,
            "*PNP0FAKE"                                                    => 1, 
            "ACPI\\MSVS010"                                                => 1,
            "*SMCB0C0"                                                     => 1,
            "USB\\VID_0BDA&PID_8187"                                       => 1,
            "SD\\VID_0000&PID_0000"                                        => 1,
            "SD\\VID_0388&PID_0000"                                        => 1,
            "PCMCIA\\PSCR-Smart_Card_Reader-488C"                          => 1,                  
            );
            
    my $modelSecStart = 0;

    # Get the linenum for the start of Model Section    
    if (defined $Globals{"ModelSection"} && $Globals{"ModelSection"} !~ /^[\s\t]*$/ && $CurrentINF->sectionDefined($Globals{"ModelSection"})) {
        $modelSecStart = $CurrentINF->getIndex($Globals{"ModelSection"}, 0) - 1;
    }    
    
    if (defined $wdkPnpIds{uc $PnpId}) {
        AddErrorRef(1515, $modelSecStart, "PnP ID", $PnpId);
    }

}

#-------------------------------------------------------------------------------------------------
#-- CheckWdfService($ServiceName, $Line)
sub CheckWdfServiceName {
    my $ServiceName = shift;
    my $Line = shift;
    my $WdfString = shift;
    
    my (%kmdfServiceNames) = ("1394VDEV"                 => 1,
            "AMCC5933"                                   => 1,
            "ECHO"                                       => 1,
            "FAKEMDM"                                    => 1,
            "HIDUSBFX2"                                  => 1,
            "KBFILTR"                                    => 1,
            "NDISEDGE"                                   => 1,
            "NDISPROT"                                   => 1,
            "OSRUSBFX2"                                  => 1,
            "GENPCI"                                     => 1,
            "PCI9X5X"                                    => 1,
            "RAMDISK"                                    => 1,
            "SERIAL"                                     => 1,
            "DYNAMBUS"                                   => 1,
            "STATBUS"                                    => 1,
            "DEVUPPER"                                   => 1,
            "WDFFEATURED"                                => 1,
            "TOASTERFILTER"                              => 1,            
            "WDFSIMPLE"                                  => 1,
            "WDFTOASTMON"                                => 1,
            "USBSAMP"                                    => 1,
            "WMISAMP"                                    => 1,
            "KMDF1394VDEV"                               => 1,
            "UMDF1394VDEV"                               => 1,
            "WUDFECHODRIVER"                             => 1,
            "WINDOWSSIDESHOWBASICDRIVER"                 => 1,
            "UMDFSKELETON"                               => 1,
            "SOCKETECHO"                                 => 1,
            "WUDFOSRUSBDRIVER"                           => 1,
            "WUDFOSRUSBFILTER"                           => 1,
            "WUDFOSRUSBFX2"                              => 1,
            "WUDFOSRUSBFX2_IDLEWAKE"                     => 1,
            "WUDFOSRUSBFX2_IMP"                          => 1,
            "WPDHELLOWORLDDRIVER"                        => 1,
            "WPDWUDFSAMPLEDRIVER"                        => 1,
            "WUDFBIOUSBSAMPLE"                           => 1,
            "WUDFTOASTER"                                => 1,
            "SENSORSKELETON"                             => 1,
            "TIMESENSOR"                                 => 1,
            "VIRTUALSERIAL"                              => 1,
            "WUDFOSRUSBFX2_IMPWPDBASICHARDWAREDRIVER"    => 1,
            "WPDMULTITRANSPORTDRIVER"                    => 1,
            "WPDSERVICESAMPLEDRIVER"                     => 1,
            "BTHECHOSAMPLESRV"                           => 1,
            "BTHECHOSAMPLECLI"                           => 1,
            "PORTIO"                                     => 1,
            "FIREFLY"                                    => 1,
            "ELOTOUCH"                                   => 1,
            "WACOMPEN"                                   => 1,
            "MOUFILTR"                                   => 1,
            "SMSCIR"                                     => 1,
            "WIFIUSB"                                    => 1,
            "MARS"                                       => 1,
            "NONPNP"                                     => 1, 
            "SCM488C"                                    => 1,              
            );

    if (defined ($kmdfServiceNames{uc $ServiceName})) {
        AddErrorRef(1515, $Line, $WdfString, $ServiceName);
    }
 
}

#-------------------------------------------------------------------------------------------------
#-- CheckUmdfCLSID($DriverClsid, $Line)
sub CheckUmdfCLSID{
    my $DriverClsid = shift;
    my $Line = shift;

    my (%umdfCLSIDs) = ("{d202f373-3aae-45ee-ae73-45766cef14b4}"        => 1,
            "{d4112073-d09b-458f-a5aa-35ef21eef5de}"        => 1,
            "{7ab7dcf5-d1d4-4085-9547-1db968cca720}"        => 1,
            "{073e9649-9d95-45c6-8ae1-06d112e97914}"        => 1,
            "{83b87d35-76b8-4920-b43c-3bde6b0ec5b8}"        => 1,
            "{7bf5cb94-b686-4721-955e-878e48933a2c}"        => 1,
            "{422d8dbc-520d-4d7e-8f53-920e5c867e6c}"        => 1,
            "{0865b2b0-6b73-428f-a3ea-2172832d6bfc}"        => 1,
            "{ec7445ee-bc00-4ced-afe7-a52849f10239}"        => 1,
            "{4f2fda86-31dd-4840-a391-7a0f29220208}"        => 1,
            "{f1cb3c15-a916-47bc-bea1-d5d4163bc6ae}"        => 1,
            "{5ed5997f-faed-4a35-ba74-fcf7b43aeba7}"        => 1,
            "{182bbb15-88c0-4e9c-a16f-eb5cdbc226c8}"        => 1,
            "{ae19511c-07b1-4476-b8eb-3351aca8e7c5}"        => 1,
            "{c8ecc087-6b79-4de5-8fb4-c03358a29617}"        => 1, 
            "{0ca6d3f4-9c49-4b81-ba8f-98f43ae6e592}"        => 1, 
            "{cfc0af02-ce72-4717-83e7-d51bcfcbe87b}"        => 1,
            );

    if (defined ($umdfCLSIDs{lc $DriverClsid})) {
        AddErrorRef(1515, $Line, "DriverCLSID", $DriverClsid);
    }    

}

#-------------------------------------------------------------------------------------------------
#-- AddPnpIdToList($Value)
sub AddPnpIdToList {
    print("Adding pnp-id to global list\n") if ($DEBUG);
    my($Value) = shift;

    $PnpIDList{uc($Value)} = TRUE;
}

#-------------------------------------------------------------------------------------------------
#-- ProcSDFiles($Section)
sub ProcSDFiles {
    print("Starting ProcSDFiles\n") if ($DEBUG);
    my($Section) =  $_[0];
    my($count)   =     0;
    my($line);
    my(@lines)    = $CurrentINF->getSection($Section);
    my($SecStart) = $CurrentINF->getIndex($Section,0)-1;
    my $Value;
    my $pathnameType;

    my($Directive,$DiskID,$SubDir,$Size);

    $CheckedSections{uc($Section)} = TRUE;


    foreach $line (@lines) {
        $count++, next if ($line =~ /^[\s\t]*$/);# Skip if it's a blank line

        ($Directive, $Value) = SplitLine($Section,$count,$line);
        ($DiskID,$SubDir,$Size) = GetList($Value,$CurrentINF->getIndex($Section,$count));

        AddWarnRef(2033,$CurrentINF->getIndex($Section, $count))
            if (   ((defined($Directive)) and ($Directive =~ /\.inf$/i))
                and not (($INTERNAL) and (uc($Module) eq "PRINTER") and ($Directive =~ /^NTPrint\.inf$/i))); # Internal Printer INFS *can* have NTPrint.INF file listed


        AddErrorRef(1034,$CurrentINF->getIndex($Section, $count))
            if ((defined($Directive)) and ($Directive =~ /\.cat$/i));

        AddErrorRef(1025,$CurrentINF->getIndex($Section, $count),"diskid")
            if (!defined($DiskID));

        AddErrorRef(1066,$CurrentINF->getIndex($Section,$count),"diskid (expected numeric value)",$Section)
            if (defined($DiskID) and $DiskID !~ /^\d*$/);

        # Added check for double backslashes since they denote a UNC path
        AddWarnRef (2192,$CurrentINF->getIndex($Section,$count))
            if (defined($SubDir) and ($SubDir =~ /^\\/ and $SubDir !~ /^\\\\/ ));

        $pathnameType = IsRelativeAbsoluteOrUnc($line);
        
        AddErrorRef(1401,$CurrentINF->getIndex($Section,$count))
            if ( $pathnameType == 1);

        AddErrorRef(1402,$CurrentINF->getIndex($Section,$count))
            if ( $pathnameType == 2);
        
        AddErrorRef(1403,$CurrentINF->getIndex($Section,$count))
            if ( $pathnameType == 3);
            
        $count++;
    }

    if ($gCopyFilesCnt == 0) {
        AddWarnRef(2207, $SecStart);
    }

    return(TRUE);
}

#-------------------------------------------------------------------------------------------------
#-- ProcSDNames($Section)
sub ProcSDNames {
    print("Starting ProcSDNames\n") if ($DEBUG);
    my($Section) =  $_[0];
    my($count)   =     0;
    my($line);
    my(@lines)    = $CurrentINF->getSection($Section);
    my($SecStart) = $CurrentINF->getIndex($Section,0)-1;
    my $Value;
    my $pathnameType;
    my($TestForNonToken, $Directive, $DiskDesc, $TagOrCabfile, $UNUSED, $Path, $Flags, $OptTag, $Directive2, $Value2);

    $CheckedSections{uc($Section)} = TRUE;

    foreach $line (@lines) {
        $count++, next if ($line =~ /^[\s\t]*$/);

        # Because our Get functions remove string tokens, we check for
        # the localizable string w/o using them
        $TestForNonToken=$line;
        $TestForNonToken=~s/.*=[\s\t]*//;
        $TestForNonToken=~s/[\s\t]*$//;

        ($DiskDesc, $TagOrCabfile,$UNUSED,$Path, $Flags, $OptTag) = split(/,/,$line);
        
        ($Directive2, $Value2) = split("=",$DiskDesc);

        $Value2 =~ s/^\s+//;
        $Value2 =~ s/\s+$//;

        AddWarnRef(2023, $CurrentINF->getIndex($Section,$count))
            if ( ((! defined($DiskDesc)) or ($DiskDesc !~ /%.*%/)) and (uc($Value2) ne "WINDOWS CD"));

        ($Directive, $Value) = SplitLine($Section,$count,$line);
        ($DiskDesc, $TagOrCabfile, $UNUSED, $Path, $Flags, $OptTag) = GetList($Value,$CurrentINF->getIndex($Section,$count));

        AddErrorRef(1066,$CurrentINF->getIndex($Section,$count),"diskid (expected numeric value)",$Section)
            if ($Directive !~ /^\d*$/);
        AddErrorRef(1025,$CurrentINF->getIndex($Section,$count),"disk-description")
            if (!defined($DiskDesc));
        # ERROR REMOVED - 01/16/02
        # AddErrorRef(1022,$CurrentINF->getIndex($Section,$count),"\"unused\" (field 3)")
        #    if (defined($UNUSED) and ($UNUSED ne "") and ($OSContext eq "Windows 2000"));
        if (defined($Flags) and ($Flags & 0x10)) {
            AddErrorRef(9999,$CurrentINF->getIndex($Section,$count)) if (! (defined $OptTag));
        }

        $pathnameType = IsRelativeAbsoluteOrUnc($line);
        
        AddErrorRef(1401,$CurrentINF->getIndex($Section,$count))
            if ( $pathnameType == 1);

        AddErrorRef(1402,$CurrentINF->getIndex($Section,$count))
            if ( $pathnameType == 2);
        
        AddErrorRef(1403,$CurrentINF->getIndex($Section,$count))
            if ( $pathnameType == 3);
            
        $count++;
    }

    if ($gCopyFilesCnt == 0) {
        AddWarnRef(2206, $SecStart);
    }

    return(TRUE);
}

#-------------------------------------------------------------------------------------------------
#-- ProcVersion($Section)
sub ProcVersion {
    print("Starting ProcVersion ($CurrentINF->{inffile})\n") if ($DEBUG);
    my($Section)  = shift;
    my(@lines)    = $CurrentINF->getSection($Section);
    my($SecStart) = $CurrentINF->getIndex($Section,0)-1;
    my($DONE)     = FALSE;
    my($line);
    my($count)    = 0;
    my($Directive, $Value);
    my($CGLine, %BadFirstPass);

    my($CLASS);

    $CheckedSections{uc($Section)} = TRUE;

    my(%VerVal)=('SIGNATURE'          , '',   # Should be $CHICAGO$
                 'CLASS'              , '',   # Should match ClassGUID
                 'CLASSGUID'          , '',   # Should match Class
                 'PROVIDER'           , '',   #
                 'CATALOGFILE'        , '',   # Used only if LayoutFile isn't defined
                 'CATALOGFILE.NT'     , '',   # Optional
                 'CATALOGFILE.NTX86'  , '',   # Optional
                 'CATALOGFILE.NTIA64' , '',   # Optional
                 'CATALOGFILE.NTAMD64', '',   # Optional
                 'DRIVERVER'          , '');  # Must be here or in each model section
                 # SetupClass may be a valid option here?

    $CheckedSections{$Section} = TRUE;

    foreach $line (@lines) {
        $count++, next if ($line =~ /^[\s\t]*$/);# Skip if it's a blank line

        ($Directive, $Value) = SplitLine($Section,$count,$line);

        # ClassGUID directive cannot reference a string in the [Strings] section.
        # Since SplitLine() replaces the reference with the string from the [Strings]
        # section, it cannot be used in the case of the ClassGUID directive. Hence, we
        # are using the standard split() function.
        if( uc($Directive) eq "CLASSGUID"){
            ($Directive, $Value) = split("=",$line);  
            
            # Remove leading and trailing whitespaces.

            $Value =~ s/^\s+//;
            $Value =~ s/\s+$//;

            $Directive =~ s/^\s+//;
            $Directive =~ s/\s+$//;
        }
        
        if (defined($VerVal{uc($Directive)})) {
            $VerVal{uc($Directive)} = $Value;
        } else {
            # Create hash of unknown directives to verify against class module
            $BadFirstPass{$Directive} = $count;
        }
        if (uc($Directive) eq "CLASS") {
                        $CLASS            = $Value;
            $VerVal{'CLASS'}  = $Value;
            $CGLine           = $CurrentINF->getIndex($Section,$count);
                        if (length($VerVal{'CLASS'}) > 32) {
                                AddErrorRef(1307, $CurrentINF->getIndex($Section,$count));
                        }
        }

        if (uc($Directive) eq "PROVIDER") {
               if (($Value =~ /MICROSOFT/i) or
                   ($Value =~ /MSFT/i)) {
                   AddErrorRef(1004, $CurrentINF->getIndex($Section, $count)) unless ($INTERNAL);
                }
                $Value =~ s/\"//g;
                if ($Value eq "") {
                    AddErrorRef(1098, $CurrentINF->getIndex($Section, $count),$Directive);
                }
        }

        if (uc($Directive) eq "SIGNATURE") {
            $Value =~ s/\"//g;
            unless ( (uc($Value) eq "\$CHICAGO\$") ||
                     (uc($Value) eq "\$WINDOWS 95\$") ||
                     (uc($Value) eq "\$WINDOWS NT\$") ) {
                AddErrorRef(1008, $CurrentINF->getIndex($Section,$count));
            }
        }

        if (uc($Directive) =~ /^CATALOG/) {
                $Value =~ s/^\s*//; # Chop leading space
                $Value =~ s/\s*$//; # Chop tailing space
                unless ($Value =~ /.{1,8}\.cat/i) {
                    AddErrorRef(1064,$CurrentINF->getIndex($Section,$count));
                }
        }

        if (uc($Directive) eq "DRIVERVER") {
                DRIVERVER($Section, $count, $Value);
        }
        $count++;
    }

    CheckClass($VerVal{'CLASS'}, $VerVal{'CLASSGUID'}, $CGLine)
        unless (($VerVal{'CLASSGUID'} eq '') || ($VerVal{'CLASS'} eq ''));

    unless ($VerVal{'CLASS'}       ne '')   { AddErrorRef(1053, $SecStart, "");}
    unless ($VerVal{'CLASSGUID'}   ne '')   { AddErrorRef(1054, $SecStart, "");}
    unless ($VerVal{'PROVIDER'}    ne '')   { AddErrorRef(1079, $SecStart, "Provider",  "Version");}
    unless (grep(m/DriverVer/i,@lines) > 0) { AddErrorRef(1080, $SecStart, "DriverVer", "Version");}

    # If we aren't looking at a system supplied inf (i.e. no LayoutFile directive) we must have a
    #   CatalogFile directive.
    unless (($VerVal{'CATALOGFILE'} ne '')
            || ($VerVal{'CATALOGFILE.NT'} ne '')
            || ($VerVal{'CATALOGFILE.NTX86'} ne '')
            || ($VerVal{'CATALOGFILE.NTIA64'} ne '')
            || ($VerVal{'CATALOGFILE.NTAMD64'} ne '')) {
        AddErrorRef(1081, $SecStart, "CatalogFile", "Version") unless ($INTERNAL);
    }

    if (($VerVal{'CLASS'} ne '')) { # && (-e "${VerVal{'CLASS'}}.pm")) {
        $ClassMod = $VerVal{'CLASS'};
    } else {
        $ClassMod = "GENERIC";
        AddWarnRef(2018, $CGLine) unless ($VerVal{'CLASS'} eq '');
    }

    # Find out the Class this INF is for
    $Module = uc($VerVal{'CLASS'});
        $Module =~ s/"//g;

    # Pull in DeviceClass Module $Module. If it doesn't exist,
    #   default to using the generic methods in the ChkInf Module
    # Unfortunately, 'require' will fail if the Module can't be found in "PATH",
    #   so explicitly check to see if it exists (which requires path info), so
    #   search each of the @INC paths for the module (ala DynaLoad package).
    #   This also allows for ChkINF to be invoked from an arbitrary 'current'
    #   directory.
    {
       my $ModFound = 0;
       my $ModPath;
       foreach $ModPath (@INC) {
          if (-e "$ModPath\\${Module}.pm") {
             require "$ModPath\\${Module}.pm";
             # Package names can't begin with numerics, so
             # refer to 1394 as FIREWIRE
             $Module="FIREWIRE"         if ($Module eq "1394");
         $Module="FIREWIREPROTOCOL" if ($Module eq "61883");
         $ModFound = 1;
             last;
          }
       }
       unless ($ModFound) {
           $Module = "GENERIC";
           require "${Module}.pm";
       }
    }

    print(STDERR "\t");
    $Module->Exists();
    $Module->InitGlobals($CurrentINF, $DCOptions);

    # Verify unknown directives against Class Module before flagging as bad
    foreach (sort(keys(%BadFirstPass))) {
            AddWarnRef(2202,$CurrentINF->getIndex("VERSION",$BadFirstPass{$_}), $_)
                unless (($_ =~ /^\s*$/) or $Module->DCDirective($_));
    }

    return(TRUE);
}

###################################################################################################
#+-----------------------------------------------------------------------------------------------+#
#|                               Subroutines to verify directives                                |#
#+-----------------------------------------------------------------------------------------------+#
###################################################################################################

#-------------------------------------------------------------------------------------------------
#-- ADDINTERFACE($Section, $index, $text)
sub ADDINTERFACE {
        # HKR = HKLM,SYSTEM\CurrentControlSet\Control\DeviceClasses\{InterfaceGUID}\
    my $Section  = shift;
    my $Offset   = shift;
    my $linetxt  = shift;
    my $line     = $CurrentINF->getIndex($Section, $Offset);


    my($IntGUID, $RefStr, $AISec, $Flags)  = GetList($linetxt,$CurrentINF->getIndex($Section,$Offset));

    if ($IntGUID !~ /\{.{8}-.{4}-.{4}-.{4}-.{12}\}/) {
        AddErrorRef(1055, $CurrentINF->getIndex($Section, $Offset));
    }

    if (defined($AISec)) {
        $AISec =~ s/^\s*//; # Remove leading spaces
        $AISec =~ s/\s*$//; # Remove trailing spaces
        if ( $CurrentINF->sectionDefined($AISec) ) {
            VerifyAddInterfaceSection($AISec);
        } else {
            AddErrorRef(1003,$CurrentINF->getIndex($Section, $Offset), $$AISec);
        }
    }

    if ((defined($Flags)) && ($Flags ne 0)) {
        AddErrorRef(1066, $CurrentINF->getIndex($Section, $Offset), "Flags (expected 0)",$Section)
            unless ($Flags =~ /^[\s\t]*$/);
    }

    return(TRUE);
}

#-------------------------------------------------------------------------------------------------
#-- VerifyAddInterfaceSection($Section)
sub VerifyAddInterfaceSection {
    print("Starting VerifyAddInterfaceSection\n") if ($DEBUG);
    my($Section) =  $_[0];
    my(@lines)    = $CurrentINF->getSection($Section);
    my($line);
    my($count)=0;

    my($Directive, $Value);

    #Valid AddInterface Directives
    my(%AddInterfaceDirective) = ("ADDREG",             1,
                                  "DELREG",             1,
                                  "BITREG",             1,
                                  "COPYFILES",          1,
                                  "DELFILES",           1,
                                  "RENFILES",           1,
                                  "UPDATEINIS",         1,
                                  "INI2REG",            1,
                                  "UPDATEINIFIELDS",    1);

    if (defined $CheckedSections{uc($Section)})  {
        return(FALSE);
    } else {
        foreach $line (@lines) {
            $count++, next if ($line =~ /^[\s\t]*$/); # Skip if it's a blank line
            ($Directive, $Value) = SplitLine($Section,$count,$line);

            # See if the Module knows about $Directives we don't before we error
            # on them.
            unless (defined($AddInterfaceDirective{uc($Directive)}))  {
                if ($Module eq "GENERIC") {
                    AddWarnRef(2202, $CurrentINF->getIndex($Section, $count),
                        "$Directive (may be device class specific)");
                } else {
                    AddWarnRef(2202, $CurrentINF->getIndex($Section, $count), $Directive)
                        unless ($Module->DCDirective($Directive));
                }
                $count++;
                next;
            }
            &{uc($Directive)}($Section, $count, $Value);
            $count++;
        }
    }
    $CheckedSections{uc($Section)} = TRUE;
    return(TRUE);
}

#-------------------------------------------------------------------------------------------------
#-- ADDREG($Section, $index, $text)
sub ADDREG {
    my $Section  = shift;
    my $Offset   = shift;
    my $linetxt  = shift;
    my $line     = $CurrentINF->getIndex($Section, $Offset);

    my(@Values)  = GetList($linetxt,$CurrentINF->getIndex($Section,$Offset));
    my($RegSection);

    foreach $RegSection (@Values) {

        if ( $CurrentINF->sectionDefined($RegSection) ) {
            VerifyAddRegistrySection($RegSection);
        } else {
            AddErrorRef(1003,$CurrentINF->getIndex($Section, $Offset),$RegSection);
        }
    }
	return(TRUE);
}

#-------------------------------------------------------------------------------------------------
# Calls chksd.exe with given argument
sub CallChkSD{
	my $Section  = shift;
	my $Offset   = shift;
	my $linetxt  = shift;
	my $sdlocation = FALSE;	
	
	#
	# Verify chksd.exe exists
	#
	if ( -e "i386\\chksd.exe" ) {
			$sdlocation = "i386\\chksd.exe";
		} elsif ( -e "chksd.exe" ) {
			$sdlocation = "chksd.exe"; 
		  } else {
				$sdlocation = FALSE;
			}
	#
	# Verify we can successfully call chksd.exe	
	#
	if (!defined($Globals{"Chksd"})) {    
		if ( $sdlocation ) {
			if ( system ($sdlocation, "-test") != 0 ) {
				AddErrorRef(1319,1);
				$Globals{"Chksd"} = FALSE;
			} else {
				$Globals{"Chksd"} = TRUE;
			  }
		} else {
			$Globals{"Chksd"} = FALSE;
		  }
	}
	#
	# Check security descriptor string
	#
	if ( $Globals{"Chksd"} == TRUE ) {   
		if ( $sdlocation ) {
			if ( system($sdlocation, $linetxt) != 0) {
				AddErrorRef(1318,$CurrentINF->getIndex($Section, $Offset));
			}
		}
	}
}

#-------------------------------------------------------------------------------------------------
#-- SECURITY($Section, $index, $text)
sub SECURITY {
    my $Section  = shift;
    my $Offset   = shift;
    my $linetxt  = shift;
    my $line     = $CurrentINF->getIndex($Section, $Offset);

    # verify linetxt is a valid security descriptor
    if ($linetxt =~ /^\"(.*)\"$/) { #"
        $linetxt = $1;
    } else {  # Descriptors must be in quotes
        AddErrorRef(1301,$CurrentINF->getIndex($Section, $Offset));
    }
    #
    # call chksd.exe to see if this is a valid descriptor
    #
    CallChkSD($Section, $Offset, $linetxt);
    return(TRUE);
}

#-------------------------------------------------------------------------------------------------
#-- VerifyAddRegistrySection($Section)
sub VerifyAddRegistrySection {
    print("Starting VerifyAddRegistrySection\n") if ($DEBUG);
    my $line;
    my $Section  =  shift;
    my @lines    =  $CurrentINF->getSection($Section);
    my $count    =  0;

    my($RegRoot, $SubKey, @SubKeys, $Name, $Flags, $Value);

    # Valid RegRoot Values
    my(%RegRootValues) = ("HKCR",   1,
                          "HKCU",   1,
                          "HKLM",   1,
                          "HKU",    1,
                          "HKR",    1);

    # Temp variable used so the Flags value can be manipulated
    #  without losing the original value of Flags
    my $TempFlags;

    # This test must go if we do per-model reporting
    if (defined $CheckedSections{uc($Section)})  {
        # Don't check a section twice.
        return(FALSE);
    } else {
        # Start checking this section.
        foreach $line (@lines) {
            #Skip blank lines even though this should never be true.
            $count++, next if ($line =~ /^[\s\t]*$/);

            # Propagate the values in the line.
            ($RegRoot, $SubKey, $Name, $Flags, $Value) = GetList($line,$CurrentINF->getIndex($Section,$count));
            $Value = 0 if (! defined $Value);

            if (uc $RegRoot eq "HKLM") {
                if ( $SubKey =~ /SYSTEM\\CURRENTCONTROLSET\\SERVICES\\.+?\\(.*)/i) {
                    AddErrorRef(1305, $CurrentINF->getIndex($Section,$count) )
                        unless 
                           (defined $1 and $1 =~ /^Parameters(\\.*)?$/i) or
                           (($SubKey =~ /SYSTEM\\CURRENTCONTROLSET\\SERVICES\\RasMan\\PPP/i) && ($Name =~ /MaxConfigure/i)) 
                           or ($SubKey =~ /SYSTEM\\CURRENTCONTROLSET\\SERVICES\\WbioSrvc\\Databases/i);
                }
            }


            #
            # Check for regkeys that are often set by mis-behaving INFs (and shouldn't be)
            #
            if (uc $RegRoot eq "HKLM") {
                # remove any quotes from the subkey
                $SubKey =~ s/\"//g; #"
                @SubKeys = split(/\\+/,$SubKey);
                #
                # Nothing should be set under HKLM, System unless...
                #
                if (defined $SubKeys[0] && uc $SubKeys[0] eq "SYSTEM") {
                    #
                    # ... it's under CurrentControlSet
                    #
                    if (defined $SubKeys[1] && uc $SubKeys[1] eq "CURRENTCONTROLSET") {

                        #
                        # Even so, "HKLM,System\CurrentControlSet\Hardware Profiles"
                        #          "HKLM,System\CurrentControlSet\Enum" and
                        #          "HKLM,System\CurrentControlSet\Control\Class"
                        #          "HKLM,System\CurrentControlSet\Control\CriticalDeviceDatabase"
                        # shouldn't have values set under them.
                        #
                        if (defined $SubKeys[2] && uc $SubKeys[2] eq "HARDWARE PROFILES") {
                            # Warn - setting reg value under HKLM,System\CurrentControlSet\Hardware Profiles
                            AddWarnRef(2214, $CurrentINF->getIndex($Section,$count));
                        } elsif (defined $SubKeys[2] && uc $SubKeys[2] eq "ENUM") {
                            # Error - setting reg value under HKLM,System\CurrentControlSet\Enum
                            AddErrorRef(1313, $CurrentINF->getIndex($Section,$count));
                        } elsif (defined $SubKeys[2] && uc $SubKeys[2] eq "CONTROL") {
                            if (defined $SubKeys[3] && uc $SubKeys[3] eq "CLASS") {
                                # Warn - setting reg value under HKLM,System\CurrentControlSet\Control\Class
                                AddWarnRef(2213, $CurrentINF->getIndex($Section,$count));
                            } elsif (defined $SubKeys[3] && uc $SubKeys[3] eq "CRITICALDEVICEDATABASE") {
                                # Warn - setting reg value under HKLM,System\CurrentControlSet\Control\CriticalDeviceDatabase
                                AddWarnRef(2237, $CurrentINF->getIndex($Section,$count));
                            }
                        }
                    } else {
                        # Error - setting reg value under HKLM,System
                        AddErrorRef(1312, $CurrentINF->getIndex($Section,$count));
                    }
                }

            }
            # Make sure $SubKey and $Flags are defined
            $SubKey = " " unless(defined($SubKey));
            $Flags  =  0  unless(defined $Flags);

            # Check RunOnce keys
            if ($SubKey =~ /Windows\\CurrentVersion\\RunOnce/) {
                AddWarnRef (2024,$CurrentINF->getIndex($Section, $count));
                AddErrorRef(1050,$CurrentINF->getIndex($Section,$count))
                    unless ($Value =~ /RunDLL32/i);
            }

            if (uc($Name) eq "NOINSTALLCLASS") {
            #   $NoInstall = $Value;
            }

            # Look for device class co-installers
            if (! $INTERNAL) {
                if ((uc $RegRoot eq "HKLM") && ($SubKey =~ /System\\CurrentControlSet\\Control\\CoDeviceInstallers/i)) {
                    AddWarnRef(2201, $CurrentINF->getIndex($Section, $count));
                }
            }

            unless (defined($RegRootValues{uc($RegRoot)}))  {
                # Invalid RegRoot, error.
                AddErrorRef(1066, $CurrentINF->getIndex($Section, $count), "RegRoot ($RegRoot)", $Section);
            }

           # 0x0003 703F =  
           # 0x0000 0001 |
           # 0x0000 0002 |  
           # 0x0000 0004 |                  OR of all possible values for $Flags.
           # 0x0000 0008 |                  Read DDK Docs for possible values and explanations.
           # 0x0000 0010 |
           # 0x0000 0020 |
           # 0x0000 1000 |
           # 0x0000 2000 |
           # 0x0000 4000 |
           # 0x0000 0000 |
           # 0x0001 0000 |
           # 0x0002 0000 |
           # 0x0001 0001 |
           # 0x0002 0001
            
            if (defined($Flags) && ($Flags !~ /^[\s\t]*$/)) {
                # Control to indicate we're done exchanging string values.

                # Convert to hex num if string is hex
                $Flags = hex($Flags) if ($Flags =~ /^0x[0-9A-F]*$/i);

                # if the least significant bit of flags is '1', then the upper 16
                #   bits should be treated as follows:
                # 0x0000???? - predefined - binary data
                # 0x0001???? - predefined - DWORD
                # 0x0002???? - predefined - NONE
                # - all of the following values are written directly to registry
                # 0x0003???? - warning? treated as binary
                # 0x0004???? - warning? treated as DWORD
                # 0x0005???? - treated as DWORD_LITTLE_ENDIAN
                # 0x0006???? - error? REG_LINK
                # 0x0007???? - error (REG_MULTI_SZ, we actually block this, 0x00010000 must be used instead)
                # 0x0008???? - error (resource list)
                # 0x0009???? - error (resource list)
                # 0x000A???? - error (requirements list)
                # 0x000B???? - QWORD
                # 0x000C???? - 0xFFFF???? Warning (registry type is not defined)
                #
                # If the least significant bit is '0', then the upper 16 bits must
                #   be one of the pre-determined values.

                if ($Flags !~ /^\d*$/) 
                {
                    # Error out if we're not numeric now.
                    AddErrorRef(1066,$CurrentINF->getIndex($Section, $count),"flags ($Flags)",$Section);
                } 
                else 
                {
			# Actually perform the OR to verify the value we have.
			if (($Flags | 0xFFFF703f) != 0xFFFF703f){
				AddErrorRef( 1066, $CurrentINF->getIndex($Section, $count), "flags ($Flags)", $Section)   
			}


                    if (defined $Name) {
                        $Name  =~ s/\"//g;
                        $Value =~ s/\"//g; #"

                        if      ((uc $Name eq "UPPERFILTERS") or
                                 (uc $Name eq "LOWERFILTERS") or
                                 (uc $Name eq "HARDWAREID")   or
                                 (uc $Name eq "COMPATIBLEIDS")) {
                                                                if ((defined $Globals{CONTEXT}) and
                                                                        ($Globals{CONTEXT} eq "DDINSTALL.HW" or
                                                                         $Globals{CONTEXT} eq "CLASSINSTALL32")) {
                                        AddErrorRef(1112,$CurrentINF->getIndex($Section, $count),"FLG_ADDREG_TYPE_MULTI_SZ")
                                        unless (($Flags & 0x00010000) == 0x00010000);
                                                                }

                                # This data is used later to verify that filters aren't controlling services
                                if (uc $Name eq "UPPERFILTERS") {
                                    push(@{$Globals{"UPPERFILTERS"}},$Value);
                                } elsif (uc $Name eq "LOWERFILTERS") {
                                    push(@{$Globals{"LOWERFILTERS"}},$Value);
                                }

                        } elsif ((uc $Name eq "DEVICEDESC") or
                                 (uc $Name eq "SERVICE") or
                                 (uc $Name eq "CLASS") or
                                 (uc $Name eq "CLASSGUID") or
                                 (uc $Name eq "DRIVER") or
                                 (uc $Name eq "MFG") or
                                 (uc $Name eq "FRIENDLYNAME") or
                                 (uc $Name eq "LOCATIONINFORMATION") or
                                 (uc $Name eq "SECURITY") or
                                 (uc $Name eq "UINUMBERDESCFORMAT") or
                                 (uc $Name eq "INSTALLER32") or
                                 (uc $Name eq "ENUMPROPPAGES32") or
                                 (uc $Name eq "ICON") or
                                 (uc $Name eq "SILENTINSTALL") or
                                 (uc $Name eq "NOINSTALLCLASS")){
                                                                if ((defined $Globals{CONTEXT}) and
                                                                        ($Globals{CONTEXT} eq "DDINSTALL.HW" or
                                                                         $Globals{CONTEXT} eq "CLASSINSTALL32")) {
                                        AddErrorRef(1112,$CurrentINF->getIndex($Section, $count),"FLG_ADDREG_TYPE_SZ")
                                        unless ($Flags == 0);
                                                                }

                                if (uc $Name eq "ICON") {
                                    AddErrorRef(1113,$CurrentINF->getIndex($Section,$count))
                                        unless ($Value =~ /^\d*$/);
                                } elsif ((uc $Name eq "SILENTINSTALL") or (uc $Name eq "NOINSTALLCLASS")) {
                                    AddErrorRef(1114,$CurrentINF->getIndex($Section,$count))
                                        unless (($Value == 1) or ($Value == 0));
                                }

                        } elsif ((uc $Name eq "CONFIGFLAGS") or
                                 (uc $Name eq "CAPABILITIES") or
                                 (uc $Name eq "UINUMBER") or
                                 (uc $Name eq "DEVICETYPE") or
                                 (uc $Name eq "EXCLUSIVE") or
                                 (uc $Name eq "DEVICECHARACTERISTICS")) {
                            if ((defined $Globals{CONTEXT}) and
                                ($Globals{CONTEXT} eq "DDINSTALL.HW" or
                                 $Globals{CONTEXT} eq "CLASSINSTALL32")) {
                                AddErrorRef(1112,$CurrentINF->getIndex($Section, $count),"FLG_ADDREG_TYPE_DWORD")
                                    unless (($Flags & 0x00010001) == 0x00010001);
                                                                }
                        }

                    }

                    if ((! defined $Name) or ($Name =~ /^\s*$/i)) {
                        if ((defined $Globals{CONTEXT}) and
                            ($Globals{CONTEXT} eq "DDINSTALL.HW" or
                             $Globals{CONTEXT} eq "CLASSINSTALL32")) {
                            # These flags are allowed even if there is no value name.  Mask them out before
                            #  checking if one of the allowed flags when there is no value name is set.
                            #
                            # 0x00004000 (FLG_ADDREG_32BITKEY) (Windows XP and later.) Make the specified change 
                            #            in the 32-bit registry. If not specified, the change is made to the native registry. 
                            # 0x00001000 (FLG_ADDREG_64BITKEY) (Windows XP and later.) Make the specified change 
                            #            in the 64-bit registry. If not specified, the change is made to the native registry. 
                            $TempFlags = ($Flags & (~(0x00004000 | 0x00001000)));

                            # 0x00000000 (FLG_ADDREG_TYPE_SZ) If name is omitted, the value-entry-name is the 
                            #            default "unnamed" value entry for this key which is a string-type
                            # 0x00000004 (FLG_ADDREG_DELVAL) Delete the given subkey from the registry, or 
                            #            delete the specified value-entry-name from the specified registry subkey. 
                            # 0x00000010 (FLG_ADDREG_KEYONLY) Create the given subkey, but ignore any supplied 
                            #            value-entry-name and/or value. 
                            # 0x00002000 (FLG_ADDREG_KEYONLY_COMMON) (Windows XP and later.) This is the same 
                            #            as FLG_ADDREG_KEYONLY but also works in a del-registry-section
                            AddErrorRef(1112,$CurrentINF->getIndex($Section, $count),"FLG_ADDREG_TYPE_SZ")
                                unless (($TempFlags == 0) or ($TempFlags == 0x00000004) or ($TempFlags == 0x00000010) or ($TempFlags == 0x00002000));
                        }
                    }

                }
            }

            # To avoid later complications.
            undef($Flags);
            # Move on to next line.
            $count++;
        }
        # Mark this section as checked.
        $CheckedSections{uc($Section)} = TRUE;
    }

    if ( $CurrentINF->sectionDefined("$Section.Security") ) {
        VerifySecurity("$Section.Security");
    }

    return(TRUE);
}

#-------------------------------------------------------------------------------------------------
#-- VerifySecurity($Section)
sub VerifySecurity {
    print("Starting VerifySecurity\n") if ($DEBUG);
    my $line;
    my $Section  =  shift;
    my @lines    =  $CurrentINF->getSection($Section);
    my $count    =  0;

    # This test must go if we do per-model reporting
    if (defined $CheckedSections{uc($Section)})  {
        # Don't check a section twice.
        return(FALSE);
    } else {
        # Start checking this section.
        foreach $line (@lines) {
            #Skip blank lines even though this should never be true.
            $count++, next if ($line =~ /^[\s\t]*$/);

            if ($line =~ /^\"(.*)\"$/) { #"
                $line = $1;
            } else {  # Descriptors must be in quotes
                AddErrorRef(1301,$CurrentINF->getIndex($Section, $count));
            }

            ($line) = SplitLine($Section,$count,$line); # Use splitline to resolve tokens
            CallChkSD($Section,$count,$line)
        }
    }
    $CheckedSections{uc($Section)} = TRUE;
    return(TRUE);
}

#-------------------------------------------------------------------------------------------------
#-- ADDSERVICE($Section, $index, $text)
# AddService=ServiceName,[flags],service-install-section[,event-log-install-section[,[EventLogType][,EventName]]]
sub ADDSERVICE {
        # HKR =  HKLM,System\CurrentControlSet\Services\ServiceName
    my $Section  = shift;
    my $Offset   = shift;
    my $linetxt  = shift;
    my $line     = $CurrentINF->getIndex($Section, $Offset);

	
    my($ServiceName, $Flags, $SISec, $ELSec, $ELType, $EName) =
        GetList($linetxt,$CurrentINF->getIndex($Section,$Offset));
    
    # Make sure a different section isn't given for the same name
    if (defined $Globals{"AllSISec"}{uc $ServiceName}) 
    {
        if ( uc($Globals{"AllSISec"}{uc $ServiceName}) ne uc($SISec) ) 
        {
            AddWarnRef(2004,$CurrentINF->getIndex($Section,$Offset),$ServiceName,$Globals{"AllSISec"}{uc $ServiceName});
        }
    } else {
        $Globals{"AllSISec"}{uc $ServiceName} = $SISec;
    }

    # Make sure a different name isn't given for the same section
    if (defined $Globals{"AllSINames"}{uc $SISec}) {
        if ( uc($Globals{"AllSINames"}{uc $SISec}) ne uc($ServiceName) ) {
            AddErrorRef(1007,$CurrentINF->getIndex($Section,$Offset),$SISec,$Globals{"AllSINames"}{uc $SISec});
        }
    } else {
        $Globals{"AllSINames"}{uc $SISec} = $ServiceName;
    }

    my(%ELTypeValues) = ("SYSTEM",      1,
                         "SECURITY",   1,
                         "APPLICATION", 1);


    # 2555 = OR of all possible values for $Flags;
    # 2555 = 0x0000001|0x00000002|0x00000004.... (See DDK Docs for possible values
    # Allowable flags:
    #   0x00000002 (SPSVCINST_ASSOCSERVICE) 
    #   0x00000008 (SPSVCINST_NOCLOBBER_DISPLAYNAME) 
    #   0x00000100 (SPSVCINST_NOCLOBBER_DESCRIPTION) 
    #   0x00000010 (SPSVCINST_NOCLOBBER_STARTTYPE) 
    #   0x00000020 (SPSVCINST_NOCLOBBER_ERRORCONTROL) 
    #   0x00000040 (SPSVCINST_NOCLOBBER_LOADORDERGROUP) 
    #   0x00000080 (SPSVCINST_NOCLOBBER_DEPENDENCIES) 
    #   0x00000001 (SPSVCINST_TAGTOFRONT) 
    #   0x00000800 (SPSVCINST_STARTSERVICE)

    if (defined($Flags) && ($Flags !~ /^[\t\s]*$/)) {

        # Convert to hex num if string is hex
        if ($Flags =~ /^0x/i) {
            $Flags =~ s/^0X//i;
            $Flags = hex($Flags);
        } elsif ((($Flags !~ /^[0-9]*$/) && ($Flags =~ /^[0-9A-F]*$/i)) || ($Flags =~ /^0/)) {
            $Flags = hex($Flags);
        }

        if ($Flags=~/^\d*$/) {
            # Force numeric strings to numeric context
            $Flags+=0;
        }

        if (! is_numeric($Flags)) {#$Flags !~ /^\d*$/) {
            # Error out if we're not numeric now.
            AddErrorRef(1066,$CurrentINF->getIndex($Section,$Offset),"flags ($Flags)","AddService");
        } else {
            if (($Flags | 0x9FB) != 0x9FB) {
                # Flags are invalid
                AddErrorRef(1066,$CurrentINF->getIndex($Section,$Offset),"flags ($Flags)","AddService");
            }
        }
    }

    undef($Flags) if (! is_numeric($Flags) );#($Flags =~ /^[\s\t]*$/) or ($Flags =~ /^$/) or ($Flags=~ /[G-Zg-z]/));

    if (((defined($Flags)) && ($Flags != 2)) or (defined($SISec))) {
        unless (defined($ServiceName) && ($ServiceName !~ /^$/)) {
            AddErrorRef(1025, $CurrentINF->getIndex($Section, $Offset), "ServiceName");
        }

        if (defined($SISec) && ($SISec !~ /^$/)) {
            if ( $CurrentINF->sectionDefined($SISec) ) 
            {
                VerifyServiceInstallSection($SISec,$ServiceName);
            } else {
                AddErrorRef(1003,$CurrentINF->getIndex($Section, $Offset), $SISec);
            }
        } else {
            AddErrorRef(1025, $CurrentINF->getIndex($Section, $Offset), "ServiceInstallSection");
        }
        if (defined($ELSec) && ($ELSec !~ /^$/)) {
            if ( $CurrentINF->sectionDefined($ELSec) ) {
                VerifyEventLogSection($ELSec);
            } else {
                AddErrorRef(1003,$CurrentINF->getIndex($Section, $Offset), $ELSec);
            }
        }
        # Verify that the EventLog type is Application, System, or Security
        if (defined($ELType) and ($ELType !~ /^\s*$/)) {
            unless (defined($ELTypeValues{uc($ELType)})) { # Convert ELType to uppercase so it can be used to access the hash
                AddErrorRef(1061, $CurrentINF->getIndex($Section, $Offset));
            }
        }

        # This data is used later to verify that filters aren't controlling services
        if (defined $Flags) {
            push(@{$Globals{"ServiceSectionFlags"}},[$ServiceName, $Flags, $CurrentINF->getIndex($Section,$Offset)]);
        } else { # Default to value 0 flag
            push(@{$Globals{"ServiceSectionFlags"}},[$ServiceName, 0, $CurrentINF->getIndex($Section,$Offset)]);
        }

    }
    
    # To avoid later complications.
    undef($Flags);

    return(TRUE);
}

#-------------------------------------------------------------------------------------------------
#-- VerifyEventLogSection($Section)
sub VerifyEventLogSection {
    print("Starting VerifyEventLogSection\n") if ($DEBUG);
    my($Section) =  $_[0];
    my(@lines)    = $CurrentINF->getSection($Section);
    my($SecStart) = $CurrentINF->getIndex($Section,0)-1;
    my($line);
    my($count)=0;

    my($Directive, $Value);

    #Valid AddInterface Directives
    my(%EventLogDirectives) =    ("ADDREG", 1,
                                  "DELREG", 1,
                                  "BITREG", 1);

    if (defined $CheckedSections{uc($Section)})  {
        return(FALSE);
    } else {

        foreach $line (@lines) {
            $count++, next if ($line =~ /^[\s\t]*$/);# Skip if it's a blank line

            ($Directive, $Value) = SplitLine($Section,$count,$line);

            unless (defined($EventLogDirectives{uc($Directive)}))  {
                if ($Module eq "GENERIC") {
                    AddWarnRef(2202, $CurrentINF->getIndex($Section, $count),
                        "$Directive (may be device class specific)");
                } else {
                    AddWarnRef(2202, $CurrentINF->getIndex($Section, $count), $Directive)
                        unless ($Module->DCDirective($Directive));
                }

                $count++;
                next;
            }

            &{uc($Directive)}($Section, $count, $Value);
            $count++;
        }
        $CheckedSections{uc($Section)} = TRUE;
    }

    return(TRUE);
}

#-------------------------------------------------------------------------------------------------
#-- VerifyServiceInstallSection($Section)
sub VerifyServiceInstallSection {
    print("Starting VerifyServiceInstallSection\n") if ($DEBUG);
    my $SISection      = shift;              # The Service Install Section listed by the AddService instruction
    my $ServiceName  = shift;             # The Name of the Service listed by the AddService instruction
    
    my $count        = 0;
    my $line;
    my @lines        = $CurrentINF->getSection($SISection);
    my $SecStart     = $CurrentINF->getIndex($SISection,0)-1;

    my($Directive, $Value, %SILine);

    #Valid RegRoot Values
    my(%SIDirectives) = ( "DISPLAYNAME",     "UNDEF",
                          "SERVICETYPE",     "UNDEF",
                          "STARTTYPE",       "UNDEF",
                          "ERRORCONTROL",    "UNDEF",
                          "SERVICEBINARY",   "UNDEF",
                          "STARTNAME",       "UNDEF",
                          "ADDREG",          "VERIFY", # Handled by subroutine
                          "DELREG",          "VERIFY", # Handled by subroutine
                          "BITREG",          "VERIFY", # Handled by subroutine
                          "SECURITY",        "VERIFY", # Handled by subroutine
                          "LOADORDERGROUP",  "UNDEF",
                          "DEPENDENCIES",    "UNDEF",
                          "DESCRIPTION",     "UNDEF");

	
    
    $CheckedSections{uc($SISection)} = TRUE;
    foreach $line (@lines) {
        $count++, next if ($line =~ /^[\s\t]*$/); # Skip if it's a blank line
        ($Directive, $Value) = SplitLine($SISection,$count,$line);

       unless (defined($SIDirectives{uc($Directive)}))  {
            AddWarnRef(2202, $CurrentINF->getIndex($SISection, $count), $Directive)
                unless ($Module->DCDirective($Directive));
            $count++;
            next;
        }

        if ($SIDirectives{uc($Directive)} eq "VERIFY") {
            &{uc($Directive)}($SISection, $count, $Value);
        } elsif (uc($Directive) eq "DESCRIPTION") {
            AddErrorRef(1078, $CurrentINF->getIndex($SISection, $count)) if (length($Value) > 1024);
        } elsif ($SIDirectives{uc($Directive)} ne "UNDEF") {
            AddErrorRef(1006, $CurrentINF->getIndex($SISection, $count), $Directive);
        } else {
            $SIDirectives{uc($Directive)} = $Value;
            $SILine{uc($Directive)}       = $CurrentINF->getIndex($SISection, $count);
        }

        if (uc $Directive eq "SERVICEBINARY") {
            if ($line !~ /=\s*\"?\%(-?\d*)\%/) { #"
                AddWarnRef(2005, $CurrentINF->getIndex($SISection,$count));
            } else {
                AddWarnRef(2005, $CurrentINF->getIndex($SISection,$count)) if ($1 eq "-1");
            }
        }

        $count++;
    }

    if ($SIDirectives{"SERVICETYPE"} ne "UNDEF") {
        # Convert to hex num if string is hex
        if ($SIDirectives{"SERVICETYPE"} =~ /^0x/i) {
            $SIDirectives{"SERVICETYPE"} =~ s/^0X//i;
            $SIDirectives{"SERVICETYPE"} = hex($SIDirectives{"SERVICETYPE"});
        } elsif ((($SIDirectives{"SERVICETYPE"} !~ /^[0-9]*$/) && ($SIDirectives{"SERVICETYPE"} =~ /^[0-9A-F]*$/i)) || ($SIDirectives{"SERVICETYPE"} =~ /^0/)) {
            $SIDirectives{"SERVICETYPE"} = hex($SIDirectives{"SERVICETYPE"});
        }

        if ($SIDirectives{"SERVICETYPE"}=~ /^\d*$/) {
            $SIDirectives{"SERVICETYPE"}+=0;
        }

        if (! is_numeric($SIDirectives{"SERVICETYPE"}) ) {
            # Error out if we're not numeric now.
            AddErrorRef(1066,$SILine{"SERVICETYPE"},"ServiceType ($SIDirectives{SERVICETYPE})", $SISection);
        } else {
            # Flags are valid- look for invalid combinations
            if ($SIDirectives{"SERVICETYPE"}&0x100) {
                # Drivers shouldn't be interactive
                AddWarnRef(2190,$SILine{"SERVICETYPE"});
            }
            if (($SIDirectives{"SERVICETYPE"}&0xF) && ($SIDirectives{"SERVICETYPE"}&0xF0)) {
                # Can't be both SERVICE_WIN32 and SERVICE_DRIVER
                AddErrorRef(1065,$SILine{"SERVICETYPE"});
            }
        }
    } else {
        AddErrorRef(1058, $SecStart, "ServiceType", $SISection);
    }


    if ($SIDirectives{"ERRORCONTROL"} ne "UNDEF") {
        $SIDirectives{"ERRORCONTROL"} = hex(lc($SIDirectives{"ERRORCONTROL"}))
            if ($SIDirectives{"ERRORCONTROL"} =~ /^0x[a-f0-9]*$/i);
        AddErrorRef(10666,$SILine{"ERRORCONTROL"},"ErrorControl ($SIDirectives{ERRORCONTROL})",$SISection)
            unless (($SIDirectives{"ERRORCONTROL"} =~ /^[0-9]*$/) &&
                   (($SIDirectives{"ERRORCONTROL"} == 0x1) ||
                    ($SIDirectives{"ERRORCONTROL"} == 0x2) ||
                    ($SIDirectives{"ERRORCONTROL"} == 0x3) ||
                    ($SIDirectives{"ERRORCONTROL"} == 0x0)));
        AddWarnRef(2101, $SILine{"ERRORCONTROL"}, $SIDirectives{"ERRORCONTROL"})
            if (($SIDirectives{"ERRORCONTROL"} =~ /^[0[xX]][a-fA-F0-9]*$/) &&
                ($SIDirectives{"ERRORCONTROL"} == 0x3));
    } else {
        AddErrorRef(1058, $SecStart, "ErrorControl", $SISection);
    }

    if ($SIDirectives{"SERVICEBINARY"} eq "UNDEF") 
    {
        AddErrorRef(1058, $SecStart, "ServiceBinary", $SISection);
    } 
    else 
    {
        if (   ($Globals{"Platform"} ne "IA64")
            && ($Globals{"Platform"} ne "AMD64")) 
        {
            # Verify a different binary wasn't given for same service name in a givem Model Section
            #
            # Bug 816 - HCT Bugs
	     # For Windows XP and later versions of NT-based operating systems, entries in the Manufacturer section can be 
	     # decorated to specify operating system versions. Different INF Models sections can be specified for different 
	     # versions of the operating system. The specified versions indicate OS versions with which the INF Models 
	     # sections will be used. If no versions are specified, Setup uses a specified Models section for all versions of 
	     # all operating systems. For more info, look for "INF Manufacturer Section" in the DDK.
	     my($ModelSection) = $Globals{"ModelSection"};

            if (defined $ServiceInstallBin{$ModelSection}{uc $ServiceName}) {
                    unless (uc $ServiceInstallBin{$ModelSection}{uc $ServiceName} eq uc $SIDirectives{"SERVICEBINARY"}){
	                    AddErrorRef(1014,$SILine{"SERVICEBINARY"},$ServiceName,$ServiceInstallBin{$ModelSection}{uc $ServiceName})
                    };
            } else {
                $ServiceInstallBin{$ModelSection}{uc $ServiceName} = $SIDirectives{"SERVICEBINARY"}
            }

            # Verify a different name wasn't given for same binary
            if (defined $Globals{"AllSINameBins"}{uc $SIDirectives{"SERVICEBINARY"}}) {
                AddErrorRef(1009,$SILine{"SERVICEBINARY"},$Globals{"AllSINameBins"}{uc $SIDirectives{"SERVICEBINARY"}},$SIDirectives{"SERVICEBINARY"})
                    unless (uc $Globals{"AllSINameBins"}{uc $SIDirectives{"SERVICEBINARY"}} eq uc $ServiceName);
            } else {
                $Globals{"AllSINameBins"}{uc $SIDirectives{"SERVICEBINARY"}} = $ServiceName;
            }
        }
    }

    if ($SIDirectives{"STARTTYPE"} ne "UNDEF") {
        $SIDirectives{"STARTTYPE"} = hex(lc($SIDirectives{"STARTTYPE"}))
            if ($SIDirectives{"STARTTYPE"} =~ /^0x[a-f0-9]*$/i);
        if ($SIDirectives{"STARTTYPE"} =~ /^\d*$/) {
            if ($SIDirectives{"STARTTYPE"} eq "UNDEF") {
                # This should be much cleaner, but will catch many of the errors popular here.
            } else {
                AddErrorRef(1066, $SILine{"STARTTYPE"}, "StartType", $SISection)
                    unless ((hex(lc($SIDirectives{"STARTTYPE"})) | 0x7) == 0x7);
                AddErrorRef(1089, $SILine{"STARTTYPE"}) if (hex(lc($SIDirectives{"STARTTYPE"})) == 0x4);
                AddWarnRef (2027, $SILine{"STARTTYPE"}, hex(lc($SIDirectives{"STARTTYPE"})),
                     "should only be specified by drivers that are required to boot.")
                     if (hex(lc($SIDirectives{"STARTTYPE"})) == 0x2);
                AddWarnRef (2027, $SILine{"STARTTYPE"}, hex(lc($SIDirectives{"STARTTYPE"})),
                    "should only be specified by drivers that may detect legacy hardware.")
                     if (hex(lc($SIDirectives{"STARTTYPE"})) == 0x1);
                AddErrorRef(1066, $SILine{"STARTTYPE"}, "StartType", "[$SISection]")
                    if (hex(lc($SIDirectives{"STARTTYPE"})) > 4);
            }
        } else {
            AddErrorRef(1066,$SILine{"STARTTYPE"},"StartType ($SIDirectives{STARTTYPE})",$SISection);
        }
    } else {
        AddErrorRef(1058, $SecStart, "StartType", $SISection);
    }

    return(TRUE);
}

#--------------------------------------------------------------------------------------------------
#-- ALSOINSTALL($Section, $index, $text)
sub ALSOINSTALL {
    my $Section  = shift;
    my $Offset   = shift;
    my $linetxt  = shift;

    if ($OSContext eq "Windows 2000") {
        AddErrorRef(1005, $CurrentINF->getIndex($Section,$Offset));
        return();
    } else {
        return();
    }
}

#-------------------------------------------------------------------------------------------------
#-- BITREG($Section, $index, $text)
sub BITREG {
    my $Section  = shift;
    my $Offset   = shift;
    my $linetxt  = shift;
    my $line     = $CurrentINF->getIndex($Section, $Offset);

    my(@Values)  = GetList($linetxt,$CurrentINF->getIndex($Section,$Offset));
    my($RegSection);

    foreach $RegSection (@Values) {
        if ( $CurrentINF->sectionDefined($RegSection) ) {
            VerifyBitRegSection($RegSection);
        } else {
            AddErrorRef(1003,$CurrentINF->getIndex($Section, $Offset), $RegSection);
        }
    }
    return(TRUE);
}

#-------------------------------------------------------------------------------------------------#
#-- VerifyBitRegSection($line, $Section) ---------------------------------------------------------#
sub VerifyBitRegSection {
    my $line;
    my $Section  =  shift;
    my @lines    =  $CurrentINF->getSection($Section);
    my $count    =  0;

    my($RegRoot, $SubKey, $Name, $Flags, $Mask, $Modify);

    #Valid RegRoot Values
    my(%RegRootValues) = ("HKCR",   1,
                          "HKCU",   1,
                          "HKLM",   1,
                          "HKU",    1,
                          "HKR",    1);

    if (defined $CheckedSections{uc($Section)})  {
        # Don't check a section twice.
        return(FALSE);
    } else {
        print(STDERR "\tBeginning VerifyBitRegSection (@_)...\n") if ($DEBUG);
        # Start checking this section.
        foreach $line (@lines) {
            #Skip blank lines even though this should never be true.
            $count++, next if ($line =~ /^[\s\t]*$/);

            GetLine($line) if ($DEBUG);
            ($RegRoot, $SubKey, $Name, $Flags, $Mask, $Modify) =
                GetList($line,$CurrentINF->getIndex($Section,$count));

            unless (defined($RegRootValues{uc($RegRoot)}))  {
                AddErrorRef(1063, $CurrentINF->getIndex($Section,$count), $RegRoot."BitReg");
                $count++;
                next;
            }

            if (defined($Flags) && ($Flags != 0) && ($Flags != 1)) {
                AddErrorRef(1068, $CurrentINF->getIndex($Section,$count), "flag. (Expected null, 0, or 1)");
            }

            unless (defined($RegRoot)) {
                AddErrorRef(1060, $CurrentINF->getIndex($Section,$count), "reg-root");
            }

            unless (defined($Name)) {
                AddErrorRef(1060, $CurrentINF->getIndex($Section,$count), "value-entry-name");
            }

            unless (defined($Mask)) {
                AddErrorRef(1060, $CurrentINF->getIndex($Section,$count), "byte-mask");
            }

            unless (defined($Modify)) {
                AddErrorRef(1060, $CurrentINF->getIndex($Section,$count), "byte-to-modify");
            }

            $count++;
        }
        $CheckedSections{$Section} = TRUE;
    }

    print(STDERR "\tExiting VerifyBitRegSection...\n") if ($DEBUG);
    return(TRUE);
}

#-------------------------------------------------------------------------------------------------
#-- GetSourceDisksFilesList() - returns array of files (lines) in SourceDisksFiles sections,
#      decorated, undecorated, and/or included via INCLUDE=.
#      String tokens are also removed from list.
#      Results are cached (per INF) for subsequent callers.
sub GetSourceDisksFilesList {
    # Might be better as an assoc array so 'grep' doesn't have to be used to
    #   search the array (grep may report false results due to substring matches).

    # Cache the results of the results of this (@CachedSDFiles) so the
    #   work only has to be done once (per INF).
    return @CachedSDFiles if ($#CachedSDFiles > 0);

    my @Decorations = (".x86", ".IA64", ".AMD64", "");  # listed in priority order
    my $Decor;

    # Build list of SourceDisksFiles, undecorated and decorated.  I think SetupAPI's
    # actually look for decorated sections first (regardless of which INF they are in)
    # so build list in that order.
    foreach $Decor (@Decorations) {
        if ( $CurrentINF->sectionDefined("SOURCEDISKSFILES" . $Decor) ) {
            @CachedSDFiles = (@CachedSDFiles, $CurrentINF->getSection("SourceDisksFiles" . $Decor));
        }
    }

    # process entire set for string tokens (if there is anything to process)
    if ($#CachedSDFiles > 0) {
        map( {$_=RemoveStringTokens(-1,$_);} @CachedSDFiles );
    } else {
        if ( ! $CurrentINF->sectionDefined("SOURCEDISKSFILES") and
             ! $CurrentINF->sectionDefined("SOURCEDISKSFILES.X86") and
             ! $CurrentINF->sectionDefined("SOURCEDISKSFILES.IA64") and
             ! $CurrentINF->sectionDefined("SOURCEDISKSFILES.AMD64") ) {
                  AddWarnRef(2111, 0);
        }
    }

    return @CachedSDFiles;
}

#-------------------------------------------------------------------------------------------------
#-- GetOrigSourceDisksFilesList() - returns array of files (lines) in SourceDisksFiles sections,
#      decorated, undecorated, but *not* from INCLUDE= infs.
#      String tokens are also removed from list.
#      Results are *not* cached for subsequent callers.
sub GetOrigSourceDisksFilesList
{
    # Might be better as an assoc array so 'grep' doesn't have to be used to
    #   search the array (grep may report false results due to substring matches).

    # Don't bother caching this (caller can cache if needed)
    my @SDFiles = ();

    my @Decorations = (".x86", ".IA64", ".AMD64", "");  # listed in priority order
    my $Decor;

    # Build list of SourceDisksFiles, undecorated and decorated.  I think SetupAPI's
    # actually look for decorated sections first (regardless of which INF they are in)
    # so build list in that order.
    foreach $Decor (@Decorations) {
        if ( $CurrentINF->OrigSectionDefined("SOURCEDISKSFILES" . $Decor) ) {
            @SDFiles = (@SDFiles, $CurrentINF->getOrigSection("SourceDisksFiles" . $Decor));
        }
    }

    # process entire set for string tokens (if there is anything to process)
    if ($#SDFiles > 0)
    {
        map( {$_=RemoveStringTokens(-1,$_);} @SDFiles );
    }

    return @SDFiles;
}

#-------------------------------------------------------------------------------------------------
#-- COPYINF($Section, $index, $text)
sub COPYINF {
    my $Section  = shift;
    my $Offset   = shift;
    my $linetxt  = shift;
    my $line     = $CurrentINF->getIndex($Section, $Offset);
    my @Values   = GetList($linetxt,$CurrentINF->getIndex($Section,$Offset));

    my $InfFile;
    my @sdfiles  = ("");

    # Build list of SourceDisksFiles
    @sdfiles = GetSourceDisksFilesList();
    if (! $#sdfiles < 0) {
        foreach $InfFile (@Values) {
            # CopyINF only allows filenames
            AddWarnRef(2112, $CurrentINF->getIndex($Section, $Offset), $InfFile)
                if ((grep(/\Q$InfFile\E/i,@sdfiles)) < 1); # Protect against names with special characters (like '$')

            AddErrorRef(1093, $CurrentINF->getIndex($Section, $Offset))
                if (uc($InfFile) eq uc($CurrentINF->{inffile}));
        }
    }
    return(TRUE);
}

#-------------------------------------------------------------------------------------------------
#-- COPYFILES($Section, $index, $text)
sub COPYFILES {
    my $Section  = shift;
    my $Offset   = shift;
    my $linetxt  = shift;
    my $line     = $CurrentINF->getIndex($Section, $Offset);
    my @Values   = GetList($linetxt,$CurrentINF->getIndex($Section,$Offset));

    my $RegSection;
    my @sdfiles  = ();
    my %DestDirs;
    my $count = 0;
    my $Directive;
    my $Value;
    my $pathnameType;


    # Build list of SourceDisksFiles
    @sdfiles = GetSourceDisksFilesList();

    # Make the destdirs hash
    if ( $CurrentINF->sectionDefined("DestinationDirs") ) {
        foreach ( $CurrentINF->getSection("DestinationDirs") ) {
           $count++, next if (/^\s*$/);
           ($Directive, $Value) = SplitLine("DestinationDirs",$count++,$_);
           $DestDirs{uc($Directive)} = $Value;
        }
    }

    foreach $RegSection (@Values) {
        # If the next identifier begins w/ an @, it specifies only one file
        if ($RegSection =~ /^@/) {
            $RegSection =~ s/^@//;

            if (! defined $DestDirs{"DEFAULTDESTDIR"}) {
                AddErrorRef(1309, $CurrentINF->getIndex($Section,$Offset));
            }
            if (! $#sdfiles < 0) {
                AddWarnRef(2112, $CurrentINF->getIndex($Section, $Offset), $RegSection)
                    if ((grep(/\Q$RegSection\E/i,@sdfiles)) < 1); # Protect against names with special characters (like '$')
            }
            AddErrorRef(1093, $CurrentINF->getIndex($Section, $Offset))
                if (uc($RegSection) eq uc($CurrentINF->{inffile}));

            $pathnameType = IsRelativeAbsoluteOrUnc($RegSection);
        
            AddErrorRef(1401,$CurrentINF->getIndex($Section,$Offset))
                if ( $pathnameType == 1);

            AddErrorRef(1402,$CurrentINF->getIndex($Section,$Offset))
                if ( $pathnameType == 2);
        
            AddErrorRef(1403,$CurrentINF->getIndex($Section,$Offset))
                if ( $pathnameType == 3);
                
        # otherwise, it specifies a copyfiles-section
        } else {
            if ( $CurrentINF->sectionDefined($RegSection) ) {
                VerifyCopyFilesSection($RegSection);
                if ( $CurrentINF->sectionDefined("$RegSection.Security") ) {
                    VerifySecurity("$RegSection.Security");
                }

            } else {
                AddErrorRef(1003,$CurrentINF->getIndex($Section, $Offset), $RegSection);
            }
        }
    }

    return(TRUE);
}

#-------------------------------------------------------------------------------------------------
#-- VerifyCopyFilesSection($Section)
sub VerifyCopyFilesSection {
    print("Starting VerifyCopyFilesSection\n") if ($DEBUG);

    my($Section) =  $_[0];
    my(@lines)    = $CurrentINF->getSection($Section);
    my($SecStart) = $CurrentINF->getIndex($Section,0)-1;
    my($line);
    my($count)=0;

    my($Destination, $Source, $Temp, $Flags);

    my(@sdfiles) = ();
    my(%DestDirs,$Directive,$Value,$thisdir);

    my $pathnameType;

        # Make the destdirs hash
        if ( $CurrentINF->sectionDefined("DestinationDirs") ) {
            foreach ( $CurrentINF->getSection("DestinationDirs") ) {
                $count++, next if (/^\s*$/);
                ($Directive, $Value) = SplitLine("DestinationDirs",$count++,$_);
                $DestDirs{uc($Directive)} = $Value;
            }
        }
        $count = 0;

        $CheckedSections{uc($Section)} = TRUE;
    
        # If SourceDisksFiles is not defined, error & exit routine
        # Build list of SourceDisksFiles
        @sdfiles = GetSourceDisksFilesList();

        foreach $line (@lines) {

            $pathnameType = IsRelativeAbsoluteOrUnc($line);
        
            AddErrorRef(1401,$CurrentINF->getIndex($Section,$count))
                if ( $pathnameType == 1);

            AddErrorRef(1402,$CurrentINF->getIndex($Section,$count))
                if ( $pathnameType == 2);
        
            AddErrorRef(1403,$CurrentINF->getIndex($Section,$count))
                if ( $pathnameType == 3);
            
            $count++, next if ($line =~ /^[\t\s]*$/);# Skip if it's a blank line

            ($Destination, $Source, $Temp, $Flags) = GetList($line,$CurrentINF->getIndex($Section,$count));

            unless ( defined $Destination ) {
                AddErrorRef(1025, $CurrentINF->getIndex($Section, $count), "destination-file-name");
                $count++;
                next;
            }

            if ((! defined ($Source)) || ($Source =~ /^[\s\t]*$/) ) {
                $Source = $Destination;
            }

            # Added check for double backslashes since they denote a UNC path
            AddWarnRef (2192,$CurrentINF->getIndex($Section,$count))
                if ($Source =~ /^\\/ and $Source !~ /^\\\\/ );

            if ($Destination =~ /.inf$/i) {
                if (defined $DestDirs{uc $Section} ) {
                    # Going to specific dest dir
                    $thisdir = $DestDirs{uc $Section};
                } elsif (defined $DestDirs{"DEFAULTDESTDIR"}) {
                    # Going to default location
                    $thisdir = $DestDirs{"DEFAULTDESTDIR"};
                } else {
                    # No location known
                    $thisdir = 0;
                }

                if (($thisdir =~ /^\s*17/) or (($thisdir =~/^\s*10,/) and ($thisdir =~/,inf/i))) {
                    AddErrorRef(1029, $CurrentINF->getIndex($Section, $count) );
                } else {
                    AddWarnRef(2028, $CurrentINF->getIndex($Section, $count) );
                }

            } elsif ($Destination =~ /.cat$/i) {
                AddErrorRef(1030, $CurrentINF->getIndex($Section, $count) );
            }

            my $filename = $Source;
            $filename = quotemeta($filename); # Protect against names with special characters (like '$')
            $filename =~ s/.$/\?/; # make last character a wildcard (to handle compressed names)
            AddWarnRef(2112, $CurrentINF->getIndex($Section, $count), $Source)
                if (grep(/$filename/i,@sdfiles) < 1);

            if (($Destination =~ /.sys$/i) and (defined $DestDirs{uc($Section)})){
                AddWarnRef(2035, $CurrentINF->getIndex($Section, $count))
                                       unless (($DestDirs{uc($Section)} =~ /^12/) or
                                                (($DestDirs{uc($Section)} =~ /^10/) and
                                                 ($DestDirs{uc($Section)} =~ /system32\\drivers/i)));
            } elsif ($Destination =~ /.sys$/i) {
                if (defined $DestDirs{'DEFAULTDESTDIR'}) {
                    my($Temp) = $DestDirs{'DEFAULTDESTDIR'};
                    $Temp =~ s/^12/$StringVals{"12"}/;
                    AddWarnRef(2034, $CurrentINF->getIndex($Section, $count), $Temp)
                       unless ($DestDirs{'DEFAULTDESTDIR'} =~ /^12/);
                } else {
                    AddErrorRef(1092, $CurrentINF->getIndex($Section, $count) );
                }
            }

            AddWarnRef (2026, $CurrentINF->getIndex($Section,$count)) if (uc($Source) eq uc($CurrentINF->{inffile}));
            # 7c7f = OR of all possible values for $Flags;
            # 7c7f = 0x0000001|0x00000002|0x00000004.... (See DDK Docs for possible values
            # Updated this value for new flag 0x00004000 ; COPYFLG_IN_USE_TRY_RENAME
            if (defined($Flags) && $Flags =~ /^(0x)[0-9A-F]*$/i) {

                $Flags = lc($Flags);

                # Try to determine if we should be reading this as a hex value
                $Flags = hex($Flags) if (($Flags =~ /^0x[0-9A-F]*$/i) || ($Flags =~ /[A-F]*/i));

                if ($Flags !~ /^\d*$/) {
                    AddErrorRef(1066,$CurrentINF->getIndex($Section, $count),"flags ($Flags)",$Section);
                } else {
                    AddErrorRef(1066,$CurrentINF->getIndex($Section, $count),"flags ($Flags)",$Section)
                        if ( ( (! $INTERNAL) && ($Flags | 0x7C7F) != 0x7C7F) ||
                             ( ($INTERNAL) && ($Flags | 0x7D7F) != 0x7D7F) ) ;
                }
            }
            undef($Flags);
            $count++;
        }

    $CheckedSections{uc($Section)} = TRUE;
    return(TRUE);
}

#-------------------------------------------------------------------------------------------------
#-- DELFILES($Section, $index, $text)
sub DELFILES {
    my $Section  = shift;
    my $Offset   = shift;
    my $linetxt  = shift;
    my $line     = $CurrentINF->getIndex($Section, $Offset);

    my(@Values)  = GetList($linetxt,$CurrentINF->getIndex($Section,$Offset));
    my($RegSection);

    foreach $RegSection (@Values) {
        if ( $CurrentINF->sectionDefined($RegSection) ) {
            VerifyDelFilesSection($RegSection);
        } else {
            AddErrorRef(1003,$line, $RegSection);
        }
    }
    return(TRUE);
}

#-------------------------------------------------------------------------------------------------
#-- VerifyDelFilesSection($Section)
sub VerifyDelFilesSection {
    print("Starting VerifyDelFilesSection\n") if ($DEBUG);
    my($Section) =  $_[0];
    my(@lines)    = $CurrentINF->getSection($Section);
    my($SecStart) = $CurrentINF->getIndex($Section,0)-1;
    my($line);
    my($count)=0;

    my($Destination, $Source, $Temp, $Flag);

    if (defined $CheckedSections{uc($Section)})  {
        return(FALSE);
    } else {
        foreach $line (@lines) {

            $count++, next if ($line =~ /^[\s\t]*$/);# Skip if it's a blank line

            ($Destination, $Source, $Temp, $Flag) = GetList($line,$CurrentINF->getIndex($Section,$count));


            $Flag=hex($Flag) if defined($Flag);

            if (defined($Flag) && (($Flag != 0x00000001) && ($Flag != 0x00010000))) {
                AddErrorRef(1066, $CurrentINF->getIndex($Section, $count),
                    "flag (Expected null, 0x00000001, or 0x00010000).",$Section);
            }
            unless (defined($Destination)) {
                AddErrorRef(1025, $CurrentINF->getIndex($Section, $count), "destination-file-name");
            }

            AddErrorRef(1031,$CurrentINF->getIndex($Section, $count))
                if ((defined($Destination)) and ($Destination =~ /\.inf$/));

            AddErrorRef(1032,$CurrentINF->getIndex($Section, $count))
                if ((defined($Destination)) and ($Destination =~ /\.cat$/));

            if (defined($Source) ) {
                AddErrorRef(1066, $CurrentINF->getIndex($Section, $count),
                    "source-file-name (expected null)", $Section)
                    unless($Source =~ /^$/);
            }

            if (defined($Temp)) {
                AddErrorRef(1066, $CurrentINF->getIndex($Section, $count),
                    "temporary-file-name (expected null)",$Section)
                    unless($Source =~ /^$/);
            }
            $count++;
        }
        $CheckedSections{uc($Section)} = TRUE;
    }
    return(TRUE);
}

#-------------------------------------------------------------------------------------------------
#-- DELREG($Section, $index, $text)
sub DELREG {
    my $Section  = shift;
    my $Offset   = shift;
    my $linetxt  = shift;
    my $line     = $CurrentINF->getIndex($Section, $Offset);

    my(@Values)  = GetList($linetxt,$CurrentINF->getIndex($Section,$Offset));
    my($RegSection);

    foreach $RegSection (@Values) {
        if ( $CurrentINF->sectionDefined($RegSection) ) {
            VerifyDelRegistrySection($RegSection);
        } else {
            AddErrorRef(1003,$line, $RegSection);
        }
    }
    return(TRUE);
}

#-------------------------------------------------------------------------------------------------
#-- VerifyDelRegistrySection($Section)
sub VerifyDelRegistrySection {
    print("Starting VerifyDelRegistrySection\n") if ($DEBUG);
    my($Section) =  $_[0];
    my(@lines)    = $CurrentINF->getSection($Section);
    my($SecStart) = $CurrentINF->getIndex($Section,0)-1;
    my($line);
    my($count)=0;

    my($RegRoot, $SubKey, $Name);

    #Valid RegRoot Values
    my(%RegRootValues) = ("HKCR",   1,
                          "HKCU",   1,
                          "HKLM",   1,
                          "HKU",    1,
                          "HKR",    1);

    if (defined $CheckedSections{uc($Section)})  {
        return(FALSE);
    } else {
        foreach $line (@lines) {

            $count++, next if ($line =~ /^[\s\t]*$/);# Skip if it's a blank line

            ($RegRoot, $SubKey, $Name) = GetList($line,$CurrentINF->getIndex($Section,$count));

            unless ( defined( $RegRootValues{uc($RegRoot)} ) )  {
                AddErrorRef(1066, $CurrentINF->getIndex($Section, $count), "RegRoot ($RegRoot)", $Section);
                $count++;
                next;
            }

            unless (defined($SubKey)) {
                AddErrorRef(1025, $CurrentINF->getIndex($Section, $count), "subkey");
            }

            $count++;
        }
        $CheckedSections{uc($Section)} = TRUE;
    }

    return(TRUE);
}

#-------------------------------------------------------------------------------------------------
#-- DELSERVICE($Section, $index, $text)
sub DELSERVICE {
    my $Section  = shift;
    my $Offset   = shift;
    my $linetxt  = shift;
    my $line     = $CurrentINF->getIndex($Section, $Offset);

    my($ServiceName, $Flags, $EventLogType, $EName) =
        GetList($linetxt,$CurrentINF->getIndex($Section,$Offset));

    my(%ELTypeValues) = ("SYSTEM",      1,
                         "SECRURITY",   1,
                         "APPLICATION", 1);

    unless (defined($ServiceName)) {
        AddErrorRef(1025, $line, "ServiceName");
    }

    if (defined($EventLogType)) {
        unless (defined($ELTypeValues{$EventLogType})) {
            AddErrorRef(1061, $line);
        }
    }

    if (defined $Flags) {

        if ($Flags=~/^\d*$/) {
            # Force numeric strings to numeric context
            $Flags+=0;
        }

        if ($Flags =~ /^0x/i) {
            $Flags =~ s/^0x//i;
            $Flags = hex($Flags);
        } elsif ($Flags =~ /^0/) {
            $Flags =~ s/^0//i;
            $Flags = oct($Flags);
        } # Otherwise, assume it's already in decimal

                # Last ditch effort
        if (! is_numeric($Flags) ) {
            $Flags=0;
        }

        # Allowable flags for the DELSERVICE directive are:
        # 
        # SPSVCINST_DELETEEVENTLOGENTRY (0x4)
        # SPSVCINST_STOPSERVICE         (0x200)
        #
        # Hence, allowable flags are 0x4 | 0x200 = 0x204
        if ( ($Flags | 0x204) != 0x204  ) {
            AddErrorRef(1066, $line, "flags ($Flags)",$Section);
        }
    }
    return(TRUE);
}

#-------------------------------------------------------------------------------------------------
#-- DEVICEUPGRADEUNSUPPORTED($Section, $index, $text)
sub DEVICEUPGRADEUNSUPPORTED {
    my $Section  = shift;
    my $Offset   = shift;
    my $linetxt  = shift;
    my $line     = $CurrentINF->getIndex($Section, $Offset);

    my(@Values)  = GetList($linetxt,$CurrentINF->getIndex($Section,$Offset));

    if (defined($Values[0])) {
        # Handle the blank case
        if ($Values[0] =~ /^[\s\t]*$/) {
            $Values[0]=0;
        }

        if ($Values[0] =~ /^0x/i) {
            $Values[0] =~ s/^0x//i;
            $Values[0] = hex($Values[0]);
        } elsif ($Values[1] =~ /^0/) {
            $Values[0] =~ s/^0//i;
            $Values[0] = oct($Values[0]);
        } # Otherwise, assume it's already in decimal

        if ($Values[0]=~/^\d*$/) {
            # Force numeric strings to numeric context
            $Values[0]+=0;
        }

        if (! is_numeric($Values[0]) ) {
            AddErrorRef(1116, $CurrentINF->getIndex($Section,$Offset), $Values[0]);
        }
    }

    if (defined $Values[1]) {
        AddErrorRef(1117, $CurrentINF->getIndex($Section,$Offset));
    }

    return(TRUE);
}

#-------------------------------------------------------------------------------------------------
#-- DRIVERVER($Section, $index, $text)
sub DRIVERVER {
    my $Section  = shift;
    my $Offset   = shift;
    my $linetxt  = shift;

    my $line     = $CurrentINF->getIndex($Section, $Offset);

    #(Month)/(Day)/(Year,Version)
    my($Line)    = $linetxt;

    my(@Values)  = split(/\//, $Line);
    my($Revision, @Versions); #Holds individual elements in version string.
    my($sec,$min,$hour,$mday,$mon,$year,$wday,$yday,$isdst) = localtime(time);
    $mon++;

    $year += 1900 if ($year < 1000); # Make sure year is 4 digits.
    if (defined($Values[2])) {
        # (Year),(Version)
        ($Values[2],$Values[3]) = GetList($Values[2],$CurrentINF->getIndex($Section,$Offset));
    }

    $Values[0] = 0 unless (defined $Values[0]);
    $Values[1] = 0 unless (defined $Values[1]);
    $Values[2] = 0 unless (defined $Values[2]);
    AddErrorRef(1066, $line, "Month","DriverVer")
        if (($Values[0] =~ /\D/) || ($Values[0] < 1) || ($Values[0] > 12));
    AddErrorRef(1086, $line, "Day","DriverVer")
        if (($Values[1] =~ /\D/) || ($Values[1] < 1) || ($Values[1] > 31));
    AddErrorRef(1069, $line)
        if (($Values[2] =~ /\D/) || ($Values[2] < 1900));

    # Month must be 2 digits
    AddErrorRef(1314, $line) unless ( length($Values[0]) == 2);
    # Day must be 2 digits
    AddErrorRef(1315, $line) unless ( length($Values[1]) == 2);
    # Year must be 4 digits
    AddErrorRef(1316, $line) unless ( length($Values[2]) == 4);

    if ($Values[0] !~ /\D/ && $Values[1] !~ /\D/ && $Values[2] !~ /\D/) {
        # These months have 31 days
        if (($Values[0] == 1) || ($Values[0] == 3) || ($Values[0] == 5) ||
            ($Values[0] == 7) || ($Values[0] == 8) || ($Values[0] ==10) ||
            ($Values[0] ==12)) {
            AddErrorRef(1082, $line, $Values[0], $Values[1], $Values[2]) if ($Values[1] > 31);

        # February follows leap year rules
        } elsif ($Values[0] == 2) {
            if (($Values[2] % 4) == 0) {
                if (($Values[2] % 100) == 0) {
                    if (($Values[2] % 400) == 0) {
                        AddErrorRef(1082, $line, $Values[0], $Values[1], $Values[2]) if ($Values[1] > 29);
                    } else {
                        AddErrorRef(1082, $line, $Values[0], $Values[1], $Values[2]) if ($Values[1] > 28);
                    }
                } else {
                    AddErrorRef(1082, $line, $Values[0], $Values[1], $Values[2]) if ($Values[1] > 29);
                }
            } else {
                AddErrorRef(1082, $line, $Values[0], $Values[1], $Values[2]) if ($Values[1] > 28);
            }

        # Which leaves us with months that only have 30 days
        } else {
            AddErrorRef(1082, $line, $Values[0], $Values[1], $Values[2]) if ($Values[1] > 30);
        }

        # Make sure the date isn't in the future!
        if ($Values[2] > $year) {
            AddErrorRef(1067, $line, "");
        } elsif ($Values[2] == $year) {
             if ($Values[0] > $mon) {
                AddErrorRef(1067, $line, "");
             } elsif ($Values[0] == $mon) {
                if ($Values[1] > $mday) {
                    AddErrorRef(1067, $line, "");
                }
            }
        }
    }

    AddWarnRef(2025, $line, "Version (DriverVer=mm/dd/yyyy,version)") if (! defined($Values[3]));

    if (defined($Values[3])) {

        @Versions=split(/\./,$Values[3]);

        foreach $Revision (@Versions) {

            $Revision =~ s/^[\s\t]*//;
            $Revision =~ s/[\s\t]*$//;

            if ($Revision !~ /^[\d\.]*$/) { # If this field contains a non-digit character.
                AddErrorRef(1012, $line,);
                next;
            }

            if (($Revision < 0) || ($Revision > 65535)) {
                AddErrorRef(1015, $line);
            }
        }

        if (defined($Versions[4])) { # Max version is w.x.y.z
            AddErrorRef(1016, $line);
        }

        if (defined($Versions[0])) {
            if (($Versions[0] =~ /^\d*$/)&&($Versions[0] == 0)) {    # w must be > 0
                AddErrorRef(1017, $line);
            }
        } else {
            AddWarnRef(2025, $line, "Version (DriverVer=mm/dd/yyyy,version)");
        }
    }

    return(TRUE);
}

#-------------------------------------------------------------------------------------------------
#-- EXCLUDEID($Section, $index, $text)
sub EXCLUDEID {
        # Nothing to check until we start maintaining per-model information.
        return(TRUE);
}

#-------------------------------------------------------------------------------------------------
#-- INCLUDE($Section, $index, $text)
sub INCLUDE {
    my $Section  = shift;
    my $Offset   = shift;
    my $linetxt  = shift;
    my $line     = $CurrentINF->getIndex($Section, $Offset);

    my $infdir   = substr($CurrentINF->{inffile},0,(rindex($CurrentINF->{inffile},"\\")));

    my (@files) = GetList($linetxt, $CurrentINF->getIndex($Section,$Offset));

    foreach (@files) {
        # INCLUDEd inf order = current dir, parent inf dir, %windir%\inf
        if ( -e "$ENV{WINDIR}\\inf\\$_") {
            $CurrentINF->addFile("$ENV{WINDIR}\\inf\\$_");
        } else {
            AddErrorRef(1002, $CurrentINF->getIndex($Section,$Offset), "$_");
        }
    }

    return(0);
}

#-------------------------------------------------------------------------------------------------
#-- INI2REG($Section, $index, $text)
sub INI2REG {
    my $Section  = shift;
    my $Offset   = shift;
    my $linetxt  = shift;
    my $line     = $CurrentINF->getIndex($Section, $Offset);

    my(@Values)  = GetList($linetxt,$CurrentINF->getIndex($Section,$Offset));
    my($RegSection);

    foreach $RegSection (@Values) {
        # BUG 596481 : log errors on all ini2reg usages
        AddErrorRef(1320,$line, $RegSection);
        if ( $CurrentINF->sectionDefined($RegSection) ) {
            VerifyIni2RegSection($RegSection);
        } else {
            AddErrorRef(1003,$line, $RegSection);
        }
    }
    return(TRUE);
}

#-------------------------------------------------------------------------------------------------#
#-- VerifyIni2RegSection($line, $Section) --------------------------------------------------------#
sub VerifyIni2RegSection {
    my($Section) =     shift;
    my($count)   =     0;
    my($line);
    my(@lines)   =     $CurrentINF->getSection($Section);

    my($IniFile, $IniSection, $IniKey, $RegRoot, $SubKey, $Flags);

    my(%RegRootValues) = ("HKCR",   1,
                          "HKCU",   1,
                          "HKLM",   1,
                          "HKU",    1,
                          "HKR",    1);

    if (defined $CheckedSections{uc($Section)})  {
        return(FALSE);
    } else {
        foreach $line (@lines) {
            $count++, next if ($line =~ /^[\s\t]*$/);# Skip if it's a blank line

            GetLine($line) if ($DEBUG);
            ($IniFile, $IniSection, $IniKey, $RegRoot, $SubKey, $Flags) = GetList($line, $CurrentINF->getIndex($Section,$count));

            unless (defined($RegRootValues{uc($RegRoot)}))  {
                AddErrorRef(1063, $CurrentINF->getIndex($Section,$count), $RegRoot);
                $count++;
                next;
            }

            if (defined($Flags) && (($Flags < 0) && ($Flags > 3))) {
                AddErrorRef(1068, $CurrentINF->getIndex($Section,$count), "flag (Expected null, 0, 1, 2, or 3).");
            }

            unless (defined($IniFile)) {
                AddErrorRef(1060, $CurrentINF->getIndex($Section,$count), "ini-file");
            }

            unless (defined($IniSection)) {
                AddErrorRef(1060, $CurrentINF->getIndex($Section,$count), "ini-section");
            }

            unless (defined($RegRoot)) {
                AddErrorRef(1060, $CurrentINF->getIndex($Section,$count), "reg-root");
            }

            unless (defined($SubKey)) {
                AddErrorRef(1060, $CurrentINF->getIndex($Section,$count), "subkey");
            }

            $count++;
        }

        $CheckedSections{$Section} = TRUE;
    }
    print(STDERR "\tExiting VerifyIni2RegSection...\n") if ($DEBUG);
    return(TRUE);
}

#-------------------------------------------------------------------------------------------------
#-- LOGCONFIG($Section, $index, $text)
sub LOGCONFIG {
    my $Section  = shift;
    my $Offset   = shift;
    my $linetxt  = shift;
    my $line     = $CurrentINF->getIndex($Section, $Offset);

    my(@Values)  = GetList($linetxt,$CurrentINF->getIndex($Section,$Offset));
    my($RegSection);

    foreach $RegSection (@Values) {
        if ( $CurrentINF->sectionDefined($RegSection) ) {
            VerifyLogConfigSection($RegSection);
        } else {
            AddErrorRef(1003,$line, $RegSection);
        }
    }
    return(TRUE);
}

#-------------------------------------------------------------------------------------------------
#-- VerifyLogConfigSection($Section)
sub VerifyLogConfigSection {
    print("Starting VerifyLogConfigSection\n") if ($DEBUG);
    my($Section) =     shift;
    my($count)   =     0;
    my($line);
    my(@lines)    = $CurrentINF->getSection($Section);
    my($SecStart) = $CurrentINF->getIndex($Section,0)-1;

    my($Directive, $Value);

    #Valid RegRoot Values
    my($expect_RHS);
    my($cnt,$i);
    my(@IRQList,@IOList,@DMAList,@MEMList);
    my(%LCDirectives) = ( "CONFIGPRIORITY", "",
                          "DMACONFIG",      "",
                          "PCCARDCONFIG",   "",
                          "IOCONFIG",       "",
                          "MEMCONFIG",      "",
                          "IRQCONFIG",      "",
                          "MFCARDCONFIG",   "",);

    my(%Priority_Value)=( 'HARDWIRED'    => 1,
                          'DESIRED'      => 1,
                          'NORMAL'       => 1,
                          'SUBOPTIMAL'   => 1,
                          'DISABLED'     => 1,
                          'RESTART'      => 1,
                          'REBOOT'       => 1,
                          'POWEROFF'     => 1,
                          'HARDRECONFIG' => 1);

    my(%ConfigType)    =( 'FORCED'       => 1,
                          'BASIC'        => 1);

    if (defined $CheckedSections{uc($Section)})  {
        return(FALSE);
    } else {
        foreach $line (@lines) {
            $count++, next if ($line =~ /^[\s\t]*$/);# Skip if it's a blank line

            ($Directive, $Value) = SplitLine($Section,$count,$line,TRUE);

            unless (defined($LCDirectives{uc($Directive)}))  {
                AddWarnRef(2202, $CurrentINF->getIndex($Section, $count), $Directive)
                    unless ($Module->DCDirective($Directive));
                $count++;
                next;
            }
            $LCDirectives{uc($Directive)} = $Value;

            # Check if ConfigPriority key.
            if (uc($Directive) eq "CONFIGPRIORITY") {

                if ($Value =~  /^([A-Z]*)[,]?([A-Z]*)$/) {

                    if (! defined($expect_RHS=$Priority_Value{$1}) ) {
                        AddErrorRef(1100,$CurrentINF->getIndex($Section, $count));
                    }
                    if ($2 !~ /^$/) {
                        AddErrorRef(1101,$CurrentINF->getIndex($Section, $count))
                            if (!defined($ConfigType{$2}));
                    }
                }
            }

            # Check if IRQCONFIG key.
            elsif ($Directive =~ /^IRQCONFIG$/i) {

		 AddErrorRef(1104,$CurrentINF->getIndex($Section, $count))
		     if ($Value !~  /^(LS:|L:)?[0-9]+\s*(,\s*[0-9]+)*$/);
            }

            # Check if IOCONFIG key.
            elsif ($Directive =~ /^IOCONFIG$/i) {

                @IOList=GetList($Value,$CurrentINF->getIndex($Section, $count),1,TRUE);
                $cnt=@IOList;
                for($i = 0; $i < $cnt; $i++) {

                    #start-end[([decode-mask]:[alias-offset]:[attr])]
                    if ($IOList[$i] =~ /^([0-9a-fA-F]*)-([0-9a-fA-F]*)[\(]?([0-9a-fA-F]*)[:]?([0-9a-fA-F]*)[:]?[M]?[\)]?$/) {
                        #Type 1 I/O Range..
                        Check64bitvalue($1,$CurrentINF->getIndex($Section, $count)); #start value
                        Check64bitvalue($2,$CurrentINF->getIndex($Section, $count)); #end value
                        Check64bitvalue($3,$CurrentINF->getIndex($Section, $count)); #decode-mask value
                        Check32bitvalue($4,$CurrentINF->getIndex($Section, $count)); #alias-offset value

                    #size@min-max[%align-mask][([decode-mask]:[alias-offset]:[attr])]
                    #RHS has to be of the form: $RHS='10@200-39f%fff0(3ff::)'

                    } elsif ($IOList[$i] =~ /^([0-9a-fA-F]*)@([0-9a-fA-F]*)-([0-9a-fA-F]*)[%]?([0-9a-fA-F]*)[\(]?([0-9a-fA-F]*)[:]?([0-9a-fA-F]*)[:]?[M]?[\)]?$/) {
                        Check32bitvalue($1,$CurrentINF->getIndex($Section, $count));  #size value
                        Check64bitvalue($2,$CurrentINF->getIndex($Section, $count));  #min value
                        Check64bitvalue($3,$CurrentINF->getIndex($Section, $count));  #max value
                        Check64bitvalue($4,$CurrentINF->getIndex($Section, $count));  #align-mask value
                        Check64bitvalue($5,$CurrentINF->getIndex($Section, $count));  #decode-mask value
                        Check32bitvalue($6,$CurrentINF->getIndex($Section, $count));  #alias-offset value
                    } else {
                        AddErrorRef(1105,$CurrentINF->getIndex($Section, $count));
                    }
                }   #for ends..
            }

            # Check if DMACONFIG key.
            elsif ($Directive =~ /^DMACONFIG$/i) {

                if ($Value =~  /^[DW]?[:]?([0-9\s,]*)$/) {

                    @DMAList=GetList($1,$CurrentINF->getIndex($Section, $count),1);
                    $cnt=@DMAList;
                    for($i = 0; $i < $cnt; $i++) {
                        AddErrorRef(1106,$CurrentINF->getIndex($Section, $count))
                            if (($DMAList[$i]>7)||($DMAList[$i]<0));
                    }
                } else {
                    AddErrorRef(1107,$CurrentINF->getIndex($Section, $count));
                }
            }

            # Check if MEMCONFIG key.
            elsif ($Directive =~ /^MEMCONFIG$/i) {

                @MEMList= GetList($Value,$CurrentINF->getIndex($Section, $count),1,TRUE);
                $cnt=@MEMList;
                for($i = 0; $i < $cnt; $i++) {
                    #start-end[(attr)]
                    if ($MEMList[$i] =~  /^([0-9a-fA-F]*)-([0-9a-fA-F]*)[\(][CDFRW]*[\)]?$/) {
                        #Type 1 I/O Range..
                        Check64bitvalue($1,$CurrentINF->getIndex($Section, $count));  #start value
                        Check64bitvalue($2,$CurrentINF->getIndex($Section, $count));  #end value

                        # size@min-max[%align-mask][(attr)]
                        # Value has to be of the form: $Value='10@200-39f%fff0(3ff::)'

                    } elsif ($MEMList[$i] =~ /^([0-9a-fA-F]*)@([0-9a-fA-F]*)-([0-9a-fA-F]*)[%]?([0-9a-fA-F]*)[\(]?[CDFRW]*[\)]?$/) {
                        Check32bitvalue($1,$CurrentINF->getIndex($Section, $count));  #size value
                        Check64bitvalue($2,$CurrentINF->getIndex($Section, $count));  #min value
                        Check64bitvalue($3,$CurrentINF->getIndex($Section, $count));  #max value
                        Check64bitvalue($4,$CurrentINF->getIndex($Section, $count));  #align-mask value
                    } elsif ($MEMList[$i] =~ /^([0-9a-fA-F]*)-([0-9a-fA-F]*)?$/) {
                        Check64bitvalue($1,$CurrentINF->getIndex($Section, $count));  #start value
                        Check64bitvalue($2,$CurrentINF->getIndex($Section, $count));  #end value
                    } else {
                        AddErrorRef(1108,$CurrentINF->getIndex($Section, $count));
                    }
                } #for ends..
            }

            $count++;
        }

        if ($LCDirectives{"CONFIGPRIORITY"} eq "") {
            AddErrorRef(1058, ($CurrentINF->getIndex($Section, 0)-1), "ConfigPriority", $Section);
        }

        $CheckedSections{uc($Section)} = TRUE;
    }

   return(TRUE);
}

#-------------------------------------------------------------------------------------------------
#-- NEEDS($Section, $index, $text, $pnpID)
sub NEEDS {
    my $Section  = shift;
    my $Offset   = shift;
    my $NeedsSection = shift;
    my $pnpID = shift;
    my $line     = $CurrentINF->getIndex($Section, $Offset);

    $CheckedSections{uc($NeedsSection)} = TRUE;

    # if including StandardHub.Dev.NT, verify this is a usb hub
    # by looking for USB\CLASS_09 in the compat pnp id

    if ( $NeedsSection =~ m/StandardHub.Dev.NT/i ) {
      if ( $pnpID !~ m/USB\\CLASS_09/i) {
         AddErrorRef(1329,$line);
      }
    }
       
       ##-- ADD ADDITIONAL CHECKING --##
    return(0);
}

#-------------------------------------------------------------------------------------------------
#-- PROFILEITEMS($Section, $index, $text)
sub PROFILEITEMS {
    my $Section  = shift;
    my $Offset   = shift;
    my $linetxt  = shift;
    my $line     = $CurrentINF->getIndex($Section, $Offset);

    my(@Values)  = GetList($linetxt,$CurrentINF->getIndex($Section,$Offset));
    my($RegSection);

    foreach $RegSection (@Values) {
        if ( $CurrentINF->sectionDefined($RegSection) ) {
            VerifyProfileItemsSection($RegSection);
        } else {
            AddErrorRef(1003,$line, $RegSection);
        }
    }
    return(TRUE);
}

#-------------------------------------------------------------------------------------------------
#-- VerifyProfileItemsSection($Section)
sub VerifyProfileItemsSection {
    print("Starting VerifyProfileItemsSection\n") if ($DEBUG);
    my($Section) =  $_[0];
    my($count)   =     0;
    my($line);
    my(@lines)    = $CurrentINF->getSection($Section);
    my($SecStart) = $CurrentINF->getIndex($Section,0)-1;

    my($Directive, $Value, @Values);
    my($CMDLINE, $NAME) = (FALSE, FALSE);

    # Valid RegRoot Values
    my(%PIDirectives) = ( "NAME",       1,
                          "CMDLINE",    1,
                          "SUBDIR",     1,
                          "WORKINGDIR", 1,
                          "ICONPATH",   1,
                          "ICONINDEX",  1,
                          "HOTKEY",     1,
                          "INFOTIP",    1);

    if (defined $CheckedSections{uc($Section)} )  {
        # Don't check a section twice.
        return(FALSE);
    } else {
        # Start checking this section.
        foreach $line (@lines) {
            $count++, next if ($line =~ /^[\s\t]*$/);

            # Propagate the values in the line.
            ($Directive, $Value) = SplitLine($Section,$count,$line);

            unless (defined($PIDirectives{uc($Directive)})) {
                AddWarnRef(2202, $CurrentINF->getIndex($Section, $count), $Directive)
                    unless ($Module->DCDirective($Directive));;
                $count++;
                next;
            }

            if      (uc($Directive) eq "NAME") {
                @Values=GetList($Value,$CurrentINF->getIndex($Section,$count));
                AddErrorRef(1088, $CurrentINF->getIndex($Section, $count), "First", "Name")
                    unless (defined($Values[0]));
                $NAME=TRUE;

            } elsif (uc($Directive) eq "CMDLINE") {
                @Values=GetList($Value,$CurrentINF->getIndex($Section,$count));
                unless (($Values[0] >= 66000) and
                        ($Values[0] <= 66004) and
                        (uc($Module) eq "PRINTER")) {
                    AddErrorRef(1044,$CurrentINF->getIndex($Section, $count),"DirID ($Values[0])",$Section)
                        unless (defined ($StringVals{$Values[0]}));
                }
                AddErrorRef(1088, $CurrentINF-getIndex($Section, $count), "Third", "CmdLine")
                    unless (defined($Values[2]));
                $CMDLINE=TRUE;

            } elsif (uc($Directive) eq "SUBDIR") {

            } elsif (uc($Directive) eq "WORKINGDIR") {
                @Values=GetList($Value,$CurrentINF->getIndex($Section,$count));
                unless (($Values[0] >= 66000) and
                        ($Values[0] <= 66004) and
                        (uc($Module) eq "PRINTER")) {
                    AddErrorRef(1044,$CurrentINF->getIndex($Section, $count),"DirID ($Values[0])",$Section)
                        unless (defined ($StringVals{$Values[0]}));
                }

            } elsif (uc($Directive) eq "ICONPATH") {
                @Values=GetList($Value,$CurrentINF->getIndex($Section,$count));
                unless (($Values[0] >= 66000) and
                        ($Values[0] <= 66004) and
                        (uc($Module) eq "PRINTER")) {
                    AddErrorRef(1044,$CurrentINF->getIndex($Section, $count),"DirID ($Values[0])",$Section)
                        unless (defined ($StringVals{$Values[0]}));
                }
                AddErrorRef(1088, $CurrentINF->getIndex($Section, $count), "Third", "IconPath")
                    unless (defined($Values[2]));

            } elsif (uc($Directive) eq "ICONINDEX") {

            } elsif (uc($Directive) eq "HOTKEY") {

            } elsif (uc($Directive) eq "INFOTIP") {

            }

            # Move on to next line.
            $count++;
        }

        AddErrorRef(1079, $CurrentINF->getIndex($Section, $count), "CmdLine", "$Section") unless ($CMDLINE);
        AddErrorRef(1080, $CurrentINF->getIndex($Section, $count), "Name", "$Section")    unless ($NAME);

        # Mark this section as checked.
        $CheckedSections{uc($Section)} = TRUE;
    }
    # Exit out of this sub.
    return(TRUE);
}

#-------------------------------------------------------------------------------------------------
# Defined so exception cases aren't needed.  Nothing to check.
#-- REBOOT
sub REBOOT { return TRUE; }

#-------------------------------------------------------------------------------------------------
#-- REGISTERDLLS($Section, $index, $text)
sub REGISTERDLLS {
    my $Section  = shift;
    my $Offset   = shift;
    my $linetxt  = shift;
    my $line     = $CurrentINF->getIndex($Section, $Offset);

    my(@Values)  = GetList($linetxt,$CurrentINF->getIndex($Section,$Offset));
    my($RegSection);

    foreach $RegSection (@Values) {
        if ( $CurrentINF->sectionDefined($RegSection) ) {
            VerifyRegisterDllsSection($RegSection);
        } else {
            AddErrorRef(1003,$line, $RegSection);
        }
    }
    return(TRUE);
}

#-------------------------------------------------------------------------------------------------
#-- VerifyRegisterDllsSection($Section)
sub VerifyRegisterDllsSection {
    my $Section = shift;
    my @lines   = $CurrentINF->getSection($Section);
    my $count   = 0;
    my $line;
    my ($dirid, $subdir, $filename, $flags, $opt_timeout, $argument);

    foreach $line (@lines) {
        $count++, next if ($line =~ /^[\s\t]*$/);

        ($dirid, $filename, $flags, $opt_timeout, $argument)
            = GetList($line, $CurrentINF->getIndex($Section,$count));

        # Make sure each element (except optional one) is defined
        foreach ($dirid, $filename, $flags) {
            AddErrorRef(1018, $CurrentINF->getIndex($Section,$count)) if (! defined $_);
        }
    }

    $CheckedSections{uc $Section} = TRUE;
    return TRUE;
}

#-------------------------------------------------------------------------------------------------
#-- RENFILES($Section, $index, $text)
sub RENFILES {
    my $Section  = shift;
    my $Offset   = shift;
    my $linetxt  = shift;
    my $line     = $CurrentINF->getIndex($Section, $Offset);

    my(@Values)  = GetList($linetxt,$CurrentINF->getIndex($Section,$Offset));
    my($RegSection);

    foreach $RegSection (@Values) {
        if ( $CurrentINF->sectionDefined($RegSection) ) {
            VerifyRenFilesSection($RegSection);
        } else {
            AddErrorRef(1003,$line, $RegSection);
        }
    }
    return(TRUE);
}

#-------------------------------------------------------------------------------------------------#
#-- VerifyRenFilesSection($line, $Section) -------------------------------------------------------#
sub VerifyRenFilesSection {
    my $Section   =  shift;
    my @lines     = $CurrentINF->getSection($Section);
    my $line;
    my $count     = 0;

    my($OldName, $NewName);

    print(STDERR "\tBeginning VerifyRenFilesSection (@_)...\n") if ($DEBUG);

    if (defined $CheckedSections{uc($Section)})  {
        print("Leaving VerifyUpdateInisSections\n") if ($DEBUG);
        return(FALSE);
    } else {
        foreach $line (@lines) {
            $count++, next if ($line =~ /^[\s\t]*$/);# Skip if it's a blank line

            GetLine($line) if ($DEBUG);
            ($NewName, $OldName) = GetList($line,$CurrentINF->getIndex($Section,$count));

            if (defined($OldName)) {
                $OldName =~ s/^[\s\t]*//;
                $OldName =~ s/[\s\t]*$//;
            } else {
                AddErrorRef(1060, $CurrentINF->getIndex($Section,$count), "old-source-file-name");
            }

            if (defined($NewName)) {
                $NewName =~ s/^[\s\t]*//;
                $NewName =~ s/[\s\t]*$//;
            } else {
                AddErrorRef(1060, $CurrentINF->getIndex($Section,$count), "new-dest-file-name");
            }

            if ((defined($OldName)) and (defined($NewName))) {
                AddErrorRef(1303, $CurrentINF->getIndex($Section,$count))
                    if (($NewName =~ /\.inf$/i) or ($OldName =~ /\.inf$/i));
                AddErrorRef(1304, $CurrentINF->getIndex($Section,$count))
                    if (($NewName =~ /\.cat$/i) or ($OldName =~ /\.cat$/i));
                AddWarnRef(2204, $CurrentINF->getIndex($Section,$count))
                    if (uc($OldName) eq uc($NewName));
            }

            $count++;
        }
        $CheckedSections{$Section} = TRUE;
    }
    print(STDERR "\tExiting VerifyRenFilesSection...\n") if ($DEBUG);
    return(TRUE);
}

#-------------------------------------------------------------------------------------------------
#-- SYSSETUPPNPFLAGS($Section, $index, $text)
sub SYSSETUPPNPFLAGS {
    my $Section  = shift;
    my $Offset   = shift;
    my $linetxt  = shift;
    my $line     = $CurrentINF->getIndex($Section, $Offset);

    my(@Values)  = GetList($linetxt,$CurrentINF->getIndex($Section,$Offset));

    if (defined($Values[0])) {
        # Handle the blank case
        if ($Values[0] =~ /^[\s\t]*$/) {
            $Values[0]=0;
        }

        if (defined ($Values[0] =~ /^0x/i)) {
            $Values[0] =~ s/^\0\x//i;
            $Values[0] = hex($Values[0]);
        } elsif ($Values[1] =~ /^0/) {
            $Values[0] =~ s/^0//i;
            $Values[0] = oct($Values[0]);
        } # Otherwise, assume it's already in decimal

        if ($Values[0]=~/^\d*$/) {
            # Force numeric strings to numeric context
            $Values[0]+=0;
        }

        if (! is_numeric($Values[0]) ) {
            $Values[0]=0;
        }

        if (($Values[0]|0x1) != 0x1) {
            AddErrorRef(1118, $CurrentINF->getIndex($Section,$Offset), $Values[0]);
        }
    }

    if (defined $Values[1]) {
        AddErrorRef(1119, $CurrentINF->getIndex($Section,$Offset));
    }

    return(TRUE);
}

#-------------------------------------------------------------------------------------------------
#-- UNREGISTERDLLS($Section, $index, $text)
sub UNREGISTERDLLS {
    my $Section  = shift;
    my $Offset   = shift;
    my $linetxt  = shift;
    my $line     = $CurrentINF->getIndex($Section, $Offset);

    my(@Values)  = GetList($linetxt,$CurrentINF->getIndex($Section,$Offset));
    my($RegSection);

    foreach $RegSection (@Values) {
        if ( $CurrentINF->sectionDefined($RegSection) ) {
            VerifyUnRegisterDllsSection($RegSection);
        } else {
            AddErrorRef(1003,$line, $RegSection);
        }
    }
    return(TRUE);
}

#-------------------------------------------------------------------------------------------------
#-- VerifyUnRegisterDllsSection($Section)
sub VerifyUnRegisterDllsSection {
    my $Section = shift;
    my @lines   = $CurrentINF->getSection($Section);
    my $count   = 0;
    my $line;
    my ($dirid, $subdir, $filename, $flags, $opt_timeout, $argument);

    foreach $line (@lines) {
        $count++, next if ($line =~ /^[\s\t]*$/);

        ($dirid, $subdir, $filename, $flags)
            = GetList($line, $CurrentINF->getIndex($Section,$count));

        # Make sure each element (except optional one) is defined
        foreach ($dirid, $filename, $flags) {
            AddErrorRef(1018, $CurrentINF->getIndex($Section,$count)) if (! defined $_);
        }
    }

        $CheckedSections{uc $Section} = TRUE;
    return TRUE;
}

#-------------------------------------------------------------------------------------------------
#-- UPDATEINIFIELDS($Section, $index, $text)
sub UPDATEINIFIELDS {
    my $Section  = shift;
    my $Offset   = shift;
    my $linetxt  = shift;
    my $line     = $CurrentINF->getIndex($Section, $Offset);

    my(@Values)  = GetList($linetxt,$CurrentINF->getIndex($Section,$Offset));
    my($RegSection);

    foreach $RegSection (@Values) {
        if ( $CurrentINF->sectionDefined($RegSection) ) {
            VerifyUpdateIniFieldsSection($RegSection);
        } else {
            AddErrorRef(1003,$line, $RegSection);
        }
    }
    return(TRUE);
}

#-------------------------------------------------------------------------------------------------#
#-- VerifyUpdateIniFieldsSection($line, $Section) ------------------------------------------------#
sub VerifyUpdateIniFieldsSection {
    my $Section  = shift;
    my @lines    = $CurrentINF->getSection($Section);
    my $count    = 0;
    my $line;

    my($IniFile, $IniSection, $Profile, $OldField, $NewField, $Flags);


    if (defined $CheckedSections{uc($Section)})  {
        return(FALSE);
    } else {
        print(STDERR "\tBeginning VerifyUpdateIniFieldsSection (@_)....\n") if ($DEBUG);
        foreach $line (@lines) {
            $count++, next if ($line =~ /^[\t\s]*$/); # Skip if it's a blank line

            GetLine($line) if ($DEBUG);
            ($IniFile, $IniSection, $Profile, $OldField, $NewField, $Flags) = GetList($line,$CurrentINF->getIndex($Section,$count));

            if (defined($Flags) && (($Flags < 0) && ($Flags > 3))) {
                AddErrorRef(1068, $CurrentINF->getIndex($Section,$count), "flag (Expected null, 0, 1, 2, or 3).");
            }

            unless (defined($IniFile)) {
                AddErrorRef(1060, $CurrentINF->getIndex($Section,$count), "ini-file");
            }

            unless (defined($IniSection)) {
                AddErrorRef(1060, $CurrentINF->getIndex($Section,$count), "ini-section");
            }

            unless (defined($Profile)) {
                AddErrorRef(1060, $CurrentINF->getIndex($Section,$count), "profile-name");
            }

            $count++;
        }
        $CheckedSections{$Section} = TRUE;
    }
    print(STDERR "\tExiting VerifyUpdateIniFieldsSection (@_)....\n") if ($DEBUG);
    return(TRUE);
}

#-------------------------------------------------------------------------------------------------
#-- UPDATEINIS($Section, $index, $text)
sub UPDATEINIS {
    my $Section  = shift;
    my $Offset   = shift;
    my $linetxt  = shift;
    my $line     = $CurrentINF->getIndex($Section, $Offset);

    my(@Values)  = GetList($linetxt,$CurrentINF->getIndex($Section,$Offset));
    my($RegSection);

    foreach $RegSection (@Values) {
        if ( $CurrentINF->sectionDefined($RegSection) ) {
            VerifyUpdateInisSection($RegSection);
        } else {
            AddErrorRef(1003,$line, $RegSection);
        }
    }
    return(TRUE);
}

#-------------------------------------------------------------------------------------------------
#-- VerifyUpdateInisSection($Section)
sub VerifyUpdateInisSection {
    print("Starting VerifyUpdateInisSections\n") if ($DEBUG);
    my $Section   =  shift;
    my @lines     = $CurrentINF->getSection($Section);
    my $SecStart  = $CurrentINF->getIndex($Section,0)-1;
    my $line;
    my $count     = 0;

    my($IniFile, $IniSection, $Old, $New, $Flags);

    if (defined $CheckedSections{uc($Section)})  {
        print("Leaving VerifyUpdateInisSections\n") if ($DEBUG);
        return(FALSE);
    } else {
        foreach $line (@lines) {
            $count++, next if ($line =~ /^[\s\t]*$/);# Skip if it's a blank line

            ($IniFile, $IniSection, $Old, $New, $Flags) =
                GetList($line,$CurrentINF->getIndex($Section,$count));

            $Flags = 0 if (! defined($Flags));
            $Flags = 0 if (($Flags =~ /^$/)||($Flags =~ /^[\s\t]*$/));

            if (defined($Flags) && (($Flags < 0) && ($Flags > 3))) {
                AddErrorRef(1066, $CurrentINF->getIndex($Section, $count),
                    "flag (Expected null, 0, 1, 2, or 3)",$Section);
            }

            unless (defined($IniFile)) {
                AddErrorRef(1025, $CurrentINF->getIndex($Section, $count), "ini-file");
            }

            unless (defined($IniSection)) {
                AddErrorRef(1025, $CurrentINF->getIndex($Section, $count), "ini-section");
            }

            $count++;
        }
        $CheckedSections{uc($Section)} = TRUE;
    }
    print("Leaving VerifyUpdateInisSections\n") if ($DEBUG);
    return(TRUE);
}

#-------------------------------------------------------------------------------------------------
#-- WIASECTION($Section, $index, $text)
sub WIASECTION {
    my $Section  = shift;
    my $Offset   = shift;
    my $linetxt  = shift;

    if ($Module eq "IMAGE") {
        IMAGE->Exists();
        IMAGE->InitGlobals($CurrentINF, $DCOptions);
        IMAGE->InfCheck();
    }
    
    return(TRUE);
}

#-------------------------------------------------------------------------------------------------
#-- FEATURESCORE($Section, $index, $text)
sub FEATURESCORE {
    my $Section  = shift;
    my $Offset   = shift;
    my $linetxt  = shift;

    if ($Module eq "DISPLAY") {
        DISPLAY->Exists();
        DISPLAY->InitGlobals($CurrentINF, $DCOptions);
        DISPLAY->InfCheck();
    }
    
    return(TRUE);
}

###################################################################################################
#+-----------------------------------------------------------------------------------------------+#
#|                               Utility sub-routines                                             #
#+-----------------------------------------------------------------------------------------------+#
###################################################################################################

# Global vars used only by utility subs
my(%TimeMarks);

#-------------------------------------------------------------------------------------------------#
# This is called by Device Class Modules to add a new error to the list of errors for the file.   #
#-- AddDeviceSpecificError($LineNum, $ErrorNum, $Error) ------------------------------------------#
sub AddDeviceSpecificError {
    my($LineNum) = $_[0];
    my($ErrorNum)= $_[1];
    my($Error)   = $_[2];
    my($str, $this_str, $SecNum);

    return(FALSE) if ($LineNum < 1);

    if (uc($Module) eq "DISPLAY") {
        $SecNum = "22.11";
    } elsif (uc($Module) eq "IMAGE") {
        $SecNum = "22.12";
    } elsif (uc($Module) eq "INFRARED") {
        $SecNum = "22.13";
    } elsif (uc($Module) eq "KEYBOARD") {
        $SecNum = "22.14";
    } elsif (uc($Module) eq "MEDIA") {
        $SecNum = "22.15";
    } elsif (uc($Module) eq "MODEM") {
        $SecNum = "22.16";
    } elsif (uc($Module) eq "MONITOR") {
        $SecNum = "22.17";
    } elsif (uc($Module) eq "MOUSE") {
        $SecNum = "22.18";
    } elsif (uc($Module) eq "NET") {
        $SecNum = "22.19";
    } elsif (uc($Module) eq "NETCLIENT") {
        $SecNum = "22.20";
    } elsif (uc($Module) eq "NETSERVICE") {
        $SecNum = "22.21";
    } elsif (uc($Module) eq "NETTRTANS") {
        $SecNum = "22.22";
    } elsif (uc($Module) eq "PRINTER") {
        $SecNum = "22.23";
    } elsif (uc($Module) eq "DISKDRIVE") {
        $SecNum = "22.24";
    } else {
        # no class specific section assigned
        $SecNum = "22.26";
    }

    $this_str = ("(DE${SecNum}.${ErrorNum}) $Error");

    if (defined($ErrorTable{$LineNum})) {
        # Don't add the same error twice, but allow dup error numbers if the text differs
        return(FALSE) if ($ErrorTable{$LineNum} =~ /\Q$this_str\E/i);
    }

    if (defined($ErrorTable{$LineNum})) {
        # If this line has errors already, append the new one.
        $str = $ErrorTable{$LineNum};
        $str = join("; ", $str, $this_str);
    } else {
        # If it doesn't have errors, nothing to append to.
        $str = "; $this_str";
    }

    # Increment the global error count
    $ErrorCount++;
    $ErrorTable{$LineNum} = $str;
    return(TRUE);
}

#-------------------------------------------------------------------------------------------------#
# This is called by Device Class Modules to add a new warning to the list of errors for the file. #
#-- AddDeviceSpecificWarning($LineNum, $ErrorNum, $Error) ----------------------------------------#
sub AddDeviceSpecificWarning {
    my($LineNum) = $_[0];
    my($WarnNum) = $_[1];
    my($Warn)    = $_[2];
    my($str, $this_str, $SecNum);

    return(FALSE) if ($LineNum < 1);

    if (uc($Module) eq "DISPLAY") {
        $SecNum = "22.11";
    } elsif (uc($Module) eq "IMAGE") {
        $SecNum = "22.12";
    } elsif (uc($Module) eq "INFRARED") {
        $SecNum = "22.13";
    } elsif (uc($Module) eq "KEYBOARD") {
        $SecNum = "22.14";
    } elsif (uc($Module) eq "MEDIA") {
        $SecNum = "22.15";
    } elsif (uc($Module) eq "MODEM") {
        $SecNum = "22.16";
    } elsif (uc($Module) eq "MONITOR") {
        $SecNum = "22.17";
    } elsif (uc($Module) eq "MOUSE") {
        $SecNum = "22.18";
    } elsif (uc($Module) eq "NET") {
        $SecNum = "22.19";
    } elsif (uc($Module) eq "NETCLIENT") {
        $SecNum = "22.20";
    } elsif (uc($Module) eq "NETSERVICE") {
        $SecNum = "22.21";
    } elsif (uc($Module) eq "NETTRTANS") {
        $SecNum = "22.22";
    } elsif (uc($Module) eq "PRINTER") {
        $SecNum = "22.23";
    } elsif (uc($Module) eq "DISKDRIVE") {
        $SecNum = "22.24";
    } else {
        # no class specific section assigned
        $SecNum = "22.26";
    }

    $this_str = ("(DW${SecNum}.${WarnNum}) $Warn");

    if (defined($WarnTable{$LineNum})) {
        # Don't add the same warning twice, but allow dup warning numbers if the text differs
        return(FALSE) if ($WarnTable{$LineNum} =~ /\Q$this_str\E/i);
    }

    if (defined($WarnTable{$LineNum})) {
        # If this line has warnings, append to new one to them
        $str = $WarnTable{$LineNum};
        $str = join("; ", $str, $this_str);
    } else {
        # Else nothing to append to.
        $str = "; $this_str";
    }

    # Increment the global warning count
    $WarnCount++;
    $WarnTable{$LineNum} = $str;
    return(TRUE);
}

#-------------------------------------------------------------------------------------------------#
# This sub adds a new error to the list of errors for the file.                                   #
#-- AddErrorRef($ErrorNum, $LineNum, @ErrorArgs) -------------------------------------------------#
sub AddErrorRef {
    my($ErrorNum) = $_[0], shift;
    my($LineNum)  = $_[0], shift;
    my(@ErrorArgs)= @_;

    return if ($LineNum < 0);

   # For neatness, don't leave this undefined
    $ErrorArgs[0] = " " if (! defined($ErrorArgs[0]));
    my($str, $this_str, $info_err, $format_err);

    if (! defined($LineNum)) {
        # Can't error a non-existant line
        warn("Error: \$LineNum not defined for ErrorRef $ErrorNum\n") if ($DEBUG);
        return(TRUE);
    }

    # Get the error
    $info_err = GetError($ErrorNum);

    if (defined($ErrorTable{$LineNum})) {
        # Don't repeat the same error!
        return(FALSE) if (($ErrorTable{$LineNum} =~ /\(E22\.1\.$ErrorNum\)/) && ($ErrorNum != 1095 && $ErrorNum != 1003));
    }

    if ( !defined($info_err) ) {
        # If we're not defined, keep the error but add this text to it.
        $this_str = "Unknown error $ErrorNum.";
    } else {
        # We are defined, so format the error
        $format_err = $$info_err[0];
        $this_str = sprintf($format_err, @ErrorArgs);
    }

    if (defined($ErrorTable{$LineNum})) {
        # If we have errors already, append to them.
        $str = $ErrorTable{$LineNum};
        $str = join("; ", $str, "(E22.1.${ErrorNum}) ".$this_str);
    } else {
        # Otherwise, create the list anew.
        $str = "; (E22.1.${ErrorNum}) ".$this_str;
    }

    # Increment the global error count.
    $ErrorCount++;
    $ErrorTable{$LineNum} = $str;

    return(TRUE);
}

#-------------------------------------------------------------------------------------------------#
# This sub adds a new warning to the list of warnings for the file.                               #
#-- AddWarnRef($WarnNum, $LineNum, @WarnArgs) ----------------------------------------------------#
sub AddWarnRef {
    my($WarnNum) = $_[0], shift;
    my($LineNum) = $_[0], shift;
    my(@WarnArgs)= @_;

    if ($WarnNum == 2014) {}
    my($str, $this_str, $info_wrn, $format_wrn);
    return if ($LineNum < 0);
    # Get the warning text
    $info_wrn = GetWarning($WarnNum);
    if (defined($WarnTable{$LineNum})) {
        # Don't repeat the same warning!
        return(FALSE) if ($WarnTable{$LineNum} =~ /\(W22\.1\.$WarnNum\)/);
    }

    if ( !defined($info_wrn) ) {
        # If there is not warning text, keep the warning but add this text.
        $this_str = "Unknown warning $WarnNum.";
    } else {
        # Otherwise format the warning.
        $format_wrn = $$info_wrn[0];
        $this_str = sprintf($format_wrn, @WarnArgs);
    }

    if (defined($WarnTable{$LineNum}))    {
        # If this line has warnings already, append the new one
        $str = $WarnTable{$LineNum};
        $str = join("; ", $str, "(W22.1.${WarnNum}) ".$this_str);
    } else {
        # Otherwise start a new list.
        $str = ("; (W22.1.${WarnNum}) ".$this_str);
    }

    # Increment the global warning count
    $WarnCount++;
    $WarnTable{$LineNum} = $str;

    return(TRUE);
}

#-------------------------------------------------------------------------------------------------#
#  Starts a timer for process $ProcName                                                           #
#-- BeginTimer($ProcName) ------------------------------------------------------------------------#
sub BeginTimer {
    my($ProcName) = @_;

    # Warn if this $ProcName exists already.
    print(STDERR "Procedure $ProcName called recursively.\n")
        if (defined($TimeMarks{$ProcName}));

    # Record the time
    $TimeMarks{$ProcName} = time();

    return($TimeMarks{$ProcName});
}

#-------------------------------------------------------------------------------------------------#
# Checks for 32-bit value                                                                         #
#-- Check32bitvalue($value, $line)----------------------------------------------------------------#
sub Check32bitvalue {
    my($num,$Lnum)=@_;
    my($len);
    $num =~ s/^[0]*//;  #Remove Starting 0s
    $len = length $num;
    AddErrorRef(1103,$Lnum) if ($len > 8);
}

#-------------------------------------------------------------------------------------------------#
# Checks for 64-bit value                                                                         #
#-- Check64bitvalue($value, $line)----------------------------------------------------------------#
sub Check64bitvalue {
    my($num,$Lnum)=@_;
    my($len);
    $num =~ s/^[0]*//;  #Remove Starting 0s
    $len = length $num;
    AddErrorRef(1103,$Lnum) if ($len > 16);
}

#-------------------------------------------------------------------------------------------------#
# Matches a CLASS with a corresponding CLASSGUID                                                  #
#-- CheckClass($ClassName, $ClassGUID, $Line) ----------------------------------------------------#
sub CheckClass {
    my($ClassName) = $_[0];
    my($ClassGUID) = $_[1];
    my($line)      = $_[2];

    $ClassName =~ s/\"//g;

    my(%ClassGUIDTable) = (
        "{6BDD1FC1-810F-11D0-BEC7-08002BE2092F}", "1394",              # 1394.pm
        "{7EBEFBC0-3200-11D2-B4C2-00A0C9697D07}", "61883",             # 61883.pm
        "{C06FF265-AE09-48F0-812C-16753D7CBA83}", "AVC",               # AVC.pm
        "{72631E54-78A4-11D0-BCF7-00AA00B7B32A}", "BATTERY",           # Battery.pm
        "{E0CBF06C-CD8B-4647-BB8A-263B43F0F974}", "BLUETOOTH",         # BlueTooth.pm
        "{4D36E965-E325-11CE-BFC1-08002BE10318}", "CDROM",             # CDROM.pm
        "{4D36E966-E325-11CE-BFC1-08002BE10318}", "COMPUTER",          # Computer.pm
        "{4D36E967-E325-11CE-BFC1-08002BE10318}", "DISKDRIVE",         # DiskDrive.pm
        "{4D36E968-E325-11CE-BFC1-08002BE10318}", "DISPLAY",           # Display.pm
        "{48721B56-6795-11D2-B1A8-0080C72E74A2}", "DOT4",              # Dot4.pm
        "{49CE6AC8-6F86-11D2-B1E5-0080C72E74A2}", "DOT4PRINT",         # Dot4Print.pm
        "{4D36E969-E325-11CE-BFC1-08002BE10318}", "FDC",               # FDC.pm
        "{4D36E980-E325-11CE-BFC1-08002BE10318}", "FLOPPYDISK",        # FloppyDisk.pm
        "{4D36E96A-E325-11CE-BFC1-08002BE10318}", "HDC",               # HDC.pm
        "{745A17A0-74D3-11D0-B6FE-00A0C90F57DA}", "HIDCLASS",          # HIDClass.pm
        "{6BDD1FC6-810F-11D0-BEC7-08002BE2092F}", "IMAGE",             # Image.pm
        "{6BDD1FC5-810F-11D0-BEC7-08002BE2092F}", "INFRARED",          # Infrared.pm
        "{4D36E96B-E325-11CE-BFC1-08002BE10318}", "KEYBOARD",          # Keyboard.pm
        "{CE5939AE-EBDE-11D0-B181-0000F8753EC4}", "MEDIUMCHANGER",     # MediumChanger.pm
        "{4D36E96C-E325-11CE-BFC1-08002BE10318}", "MEDIA",             # Media.pm
        "{4D36E96D-E325-11CE-BFC1-08002BE10318}", "MODEM",             # Modem.pm
        "{4D36E96E-E325-11CE-BFC1-08002BE10318}", "MONITOR",           # Monitor.pm
        "{4D36E96F-E325-11CE-BFC1-08002BE10318}", "MOUSE",             # Mouse.pm
        "{4D36E970-E325-11CE-BFC1-08002BE10318}", "MTD",               # MTD.pm
        "{4D36E971-E325-11CE-BFC1-08002BE10318}", "MULTIFUNCTION",     # Multifunction.pm
        "{50906CB8-BA12-11D1-BF5D-0000F805F530}", "MULTIPORTSERIAL",   # MultiportSerial.pm
        "{4D36E972-E325-11CE-BFC1-08002BE10318}", "NET",               # Net.pm
        "{4D36E973-E325-11CE-BFC1-08002BE10318}", "NETCLIENT",         # NetClient.pm
        "{4D36E974-E325-11CE-BFC1-08002BE10318}", "NETSERVICE",        # NetService.pm
        "{4D36E975-E325-11CE-BFC1-08002BE10318}", "NETTRANS",          # NetTrans.pm
        "{4D36E977-E325-11CE-BFC1-08002BE10318}", "PCMCIA",            # PCMCIA.pm
        "{4D36E978-E325-11CE-BFC1-08002BE10318}", "PORTS",             # Ports.pm
        "{4D36E979-E325-11CE-BFC1-08002BE10318}", "PRINTER",           # Printer.pm
        "{4658EE7E-F050-11D1-B6BD-00C04FA372A7}", "PNPPRINTERS",       # PnpPrinters.pm
        "{50127DC3-0F36-415E-A6CC-4CB3BE910B65}", "PROCESSOR",         # Processor.pm
        "{D48179BE-EC20-11D1-B6B8-00C04FA372A7}", "SBP2",              # SBP2.pm
        "{4D36E97B-E325-11CE-BFC1-08002BE10318}", "SCSIADAPTER",       # SCSIAdapter.pm
        "{268C95A1-EDFE-11D3-95C3-0010DC4050A5}", "SECURITYACCELERATOR",        # SecurityAccelerator.pm
        "{990A2BD7-E738-46C7-B26F-1CF8FB9F1391}", "SMARTCARD",         # SmartCard
	"{50DD5230-BA8A-11D1-BF5D-0000F805F530}", "SMARTCARDREADER",   # SmartCardReader.pm
        "{4D36E97D-E325-11CE-BFC1-08002BE10318}", "SYSTEM",            # System.pm
        "{6D807884-7D21-11CF-801C-08002BE10318}", "TAPEDRIVE",         # TapeDrive.pm;
        "{36FC9E60-C465-11CF-8056-444553540000}", "USB",               # USB.pm
        "{71A27CDD-812A-11D0-BEC7-08002BE2092F}", "VOLUME",            # Volume.pm
        "{25DBCE51-6C8F-4A72-8A6D-B54C2B4FC835}", "WCEUSBS",           # WCEUSBS.pm
        "{53D29EF7-377C-4D14-864B-EB3A85769359}", "BIOMETRIC",
        "{EEC5AD98-8080-425F-922A-DABF3DE3F69A}", "WPD",               # WPD.pm
        # File system filters
        "{B86DFF51-A31E-4BAC-B3CF-E8CFE75C9FC2}", "ACTIVITYMONITOR",
        "{FE8F1572-C67A-48C0-BBAC-0B5C6D66CAFB}", "UNDELETE",
        "{B1D1A169-C54F-4379-81DB-BEE7D88D7454}", "ANTIVIRUS",
        "{48D3EBC4-4CF8-48FF-B869-9C68AD42EB9F}", "REPLICATION",
        "{71AA14F8-6FAD-4622-AD77-92BB9D7E6947}", "CONTINUOUSBACKUP",
        "{3E3F0674-C83C-4558-BB26-9820E1EBA5C5}", "CONTENTSCREENER",
        "{8503C911-A6C7-4919-8F79-5028F5866B0C}", "QUOTAMANAGEMENT",
        "{CDCF0939-B75B-4630-BF76-80F7BA655884}", "CFSMETADATASERVER",
        "{F3586BAF-B5AA-49B5-8D6C-0569284C639F}", "COMPRESSION",
        "{A0A701C0-A511-42FF-AA6C-06DC0395576F}", "ENCRYPTION",
        "{6A0A8E78-BBA6-4FC4-A709-1E33CD09D67E}", "PHYSICALQUOTAMANAGEMENT",
        "{F8ECAFA6-66D1-41A5-899B-66585D7216B7}", "OPENFILEBACKUP",
        "{D02BC3DA-0C8E-4945-9BD5-F1883C226C8C}", "SECURITYENHANCER",
        "{D546500A-2AEB-45F6-9482-F4B1799C3177}", "HSM",
        "{89786FF1-9C12-402F-9C9E-17753C7F4375}", "COPYPROTECTION",
        );

    my(%BadClassGUIDTable) = (
        "{66F250D6-7801-4A64-B139-EEA80A450B24}", "1394DEBUG",      #reserved
        "{4D36E964-E325-11CE-BFC1-08002BE10318}", "ADAPTER",        #obsolete.
        "{D45B1C18-C8FA-11D1-9F77-0000F805F530}", "APMSUPPORT",     #obsolete.
        "{6BDD1FC2-810F-11D0-BEC7-08002BE2092F}", "DECODER",        #reserved for future use.
        "{C459DF55-DB08-11D1-B009-00A0C9081FF6}", "ENUM1394",       #reserved
        "{6BDD1FC3-810F-11D0-BEC7-08002BE2092F}", "GPS",            #reserved for future use.
        "{8ECC055D-047F-11D1-A537-0000F8753ED1}", "LEGACYDRIVER",   #reserved for system use.
        "{D45B1C18-C8FA-11D1-9F77-0000F805F530}", "NTAPM",          #reserved for system use.
        "{4D36E976-E325-11CE-BFC1-08002BE10318}", "NODRIVER",       #obsolete.
        "{4D36E97E-E325-11CE-BFC1-08002BE10318}", "OTHER DEVICES",  #reserved for system use.
        "{4D36E97A-E325-11CE-BFC1-08002BE10318}", "PRINTERUPGRADE", #reserved for system use.
        "{4D36E97C-E325-11CE-BFC1-08002BE10318}", "SOUND",          #obsolete.
        "{4D36E97E-E325-11CE-BFC1-08002BE10318}", "UNKNOWN",        #reserved for system use.
        "{533C5B84-EC70-11D2-9505-00C04F79DEAF}", "VOLUMESNAMSHOT", #reserved for system use.
        # File system filters
        "{5D1B9AAA-01E2-46AF-849F-272B3F324C46}", "SYSTEM",         #reserved for system use.
        "{E55FA6F9-128C-4D04-ABAB-630C74B1453A}", "INFRASTRUCTURE", #reserved for system use.
        );

    if (defined($ClassGUIDTable{uc($ClassGUID)})) {

        # System defined classes shouldn't usually specify a ClassInstaller
       if (   (!$INTERNAL)
           || (uc $ClassName ne "PRINTER")) # Internal Printer INFS *can* have a ClassInstaller
       {
            AddWarnRef(2077, $CurrentINF->getIndex("ClassInstall32",0)-1)
                if ($CurrentINF->sectionDefined('CLASSINSTALL32'));
            AddWarnRef(2077, $CurrentINF->getIndex("ClassInstall32.NT",0)-1)
                if ($CurrentINF->sectionDefined('CLASSINSTALL32.NT'));
            AddWarnRef(2077, $CurrentINF->getIndex("ClassInstall32.NTX86",0)-1)
                if ($CurrentINF->sectionDefined('CLASSINSTALL32.NTX86'));
            AddWarnRef(2077, $CurrentINF->getIndex("ClassInstall32.NTIA64",0)-1)
                if ($CurrentINF->sectionDefined('CLASSINSTALL32.NTIA64'));
            AddWarnRef(2077, $CurrentINF->getIndex("ClassInstall32.NTAMD64",0)-1)
                if ($CurrentINF->sectionDefined('CLASSINSTALL32.NTAMD64'));
       }

        if (uc $ClassGUIDTable{uc $ClassGUID} eq uc $ClassName) {
             # Return true if Class matches ClassGUID
            return($ClassName);
        } else {
            # Return false if they don't BUT the class is defined.
            AddErrorRef(1010, $line, $ClassGUIDTable{uc($ClassGUID)});
            return($ClassGUIDTable{uc($ClassGUID)});
        }

    } else {
        if (defined ($BadClassGUIDTable{uc($ClassGUID)})) {
            # Flag an error if the Class is on the Bad Class List
            AddErrorRef(1059, $line, $ClassGUIDTable{uc($ClassGUID)});
            return($ClassGUIDTable{uc($ClassGUID)});
         }

         if ($ClassGUID !~ /\{[0-9a-f]{8}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{4}-[0-9a-f]{12}\}/i) {
            # Class isn't defined (anywhere), and ClassGUID is in the wrong format.
            AddErrorRef(1055, $line, "");

            # Check to see if this is a string reference
            if($ClassGUID =~ m/^%/ && $ClassGUID =~ m/%$/){
                AddErrorRef(1056, $line, "");
            }
            
            return($ClassName);
         } else {
            # Class isn't known but ClasssGUID looks like it should be okay
            AddWarnRef (2215, $line, $ClassName, $ClassGUID);

            AddWarnRef(2203, $line)
               unless ( ($CurrentINF->sectionDefined('CLASSINSTALL32'))
                     or ($CurrentINF->sectionDefined('CLASSINSTALL32.NT'))
                     or ($CurrentINF->sectionDefined('CLASSINSTALL32.NTX86'))
                     or ($CurrentINF->sectionDefined('CLASSINSTALL32.NTIA64'))
                     or ($CurrentINF->sectionDefined('CLASSINSTALL32.NTAMD64')));
            return($ClassName);


         }
        # We should _NEVER_ hit this error!
        AddErrorRef(9999, $line);
        warn("Unexpected error on line ".__LINE__) if ($DEBUG);
        return($ClassName);
    }
}

#-------------------------------------------------------------------------------------------------#
#  Stops the $ProcName Timer and reports $Elapsed Time                                            #
#-- EndTimer($ProcName) --------------------------------------------------------------------------#
sub EndTimer {
    my($ProcName) = @_;
    my($EndTime);

    if (! defined($TimeMarks{$ProcName})) {
        # There's an error if BeginTimer wasn't called.
        print(STDERR "BeginTimer for procedure $ProcName not called.\n");
        return(0);
    }

    $EndTime = time();

    # Calculate and print total runtime
    my($Elapsed) = $EndTime-$TimeMarks{$ProcName};

    printf(STDERR  "Elapsed time : %02dh %02dm %02ds\n",
                    int($Elapsed / 3600), int($Elapsed / 60) % 60, $Elapsed % 60);

    undef ($TimeMarks{$ProcName});
    return($EndTime);
}

#-------------------------------------------------------------------------------------------------#
#  Returns the directive part (L-Side) of an INF line, full line if no '='                        #
#-- GetDirective($SectionName, $SectionOffset, $INFLine) -----------------------------------------#
sub GetDirective {
    my($Section)  = $_[0];
    my($Offset)   = $_[1];
    my $INFLine   = $_[2];
    my(@Count);
    my($string);

        if ($DEBUG) {
            warn("Line: ",__LINE__," $Section [Offset: $Offset] not defined\n")
                unless ( defined($Section) and defined($Offset) );
        }
    return($INFLine) unless ($INFLine =~ /=/);# There is not a directive if there isn't an '='

    $INFLine =~ s/=.*$/=/;                    # eat right part
    $INFLine =~ s/^\s*//;                     # eat leading spaces
    $INFLine =~ s/\s*=$//;                    # eat trailing spaces & =

    # Use GetList() so string tokens are swapped out before splitting on /,/ to
    #   determine if there are more than one token on the LHS
    @Count = GetList($INFLine,$CurrentINF->getIndex($Section,$Offset));
    if (defined $Count[1]) {
#        AddErrorRef(1013, $CurrentINF->getIndex($Section,$Offset))
    }

    return($INFLine);
}

#-------------------------------------------------------------------------------------------------#
#  Returns the directive value part (R-Side) of an INF line                                       #
#-- GetDirectiveValue($SectionName, $SectionOffset, $INFLine) ------------------------------------#
sub GetDirectiveValue {
    my $Section   = shift;
    my $Offset    = shift;
    my $INFLine   = shift;


    die("\$Section undefined\n") unless defined $Section;
    die("\$Offset undefined\n") unless defined $Offset;
    die("\$INFLine ($Section $Offset) undefined\n") unless defined $INFLine;

    my($TempVar);
    my($UnalteredVar);


    AddErrorRef(1087, $CurrentINF->getIndex($Section,$Offset))
        if (($INFLine =~ m/=/) && ($INFLine !~ m/=\w*/));

    return("") if ($INFLine !~ /=/);

    $INFLine =~ s/^.*=/=/;          # eat left part
    $INFLine =~ s/\s*$//;           # eat trailing spaces
    $INFLine =~ s/^=\s*//;          # eat leading spaces & =

    if ($INFLine =~ /^$/) { AddErrorRef(1087, $CurrentINF->getIndex($Section,$Offset));}

    return($INFLine); #return value
}

#-------------------------------------------------------------------------------------------------#
#-- GetError($ErrorNum) --------------------------------------------------------------------------#
sub GetError {
     my($ErrorNum) = $_[0];

     my(%ErrorTable) = (
        1001 => ["This is not a valid driver INF."],
        1002 => ["Unable to include %s. Note that only system provided INFs may be specified with the INCLUDE directive."],
        1003 => ["Section [%s] not defined."],
        1004 => ["Provider listed is Microsoft, but should be the organization who wrote this INF."],
        1005 => ["The ALSOINSTALL directive is not valid in Windows NT/2000 specific sections."],
        1006 => ["Key %s redefined"],
        1007 => ["Section [%s] already given for service named %s."],
        1008 => ["Signature is expected to be \"\$CHICAGO\$\",\"\$WINDOWS 95\$\"  or \"\$WINDOWS NT\$\""],
        1009 => ["Cannot use same binary %s for multiple services - already used for service named %s."],

        1010 => ["ClassName is expected to be \"%s\"."],
        1011 => ["There can not be more than one controlling service - use SPSVCINST_ASSOCSERVICE ".
                 "for one AddService entry only."],
        1012 => ["Version must be in the format w[.x.y.z] where w, x, y, and z are each decimal values ".
                 "of one word in size."],
        1013 => ["Not more than one token on the left of \"=\"."],
        1014 => ["Service %s already using binary %s."],
        1015 => ["Value out of range.  Must be (0 < value < 65535)."],
        1016 => ["Too many segments in revision.  Max is 4 (w.x.y.z)."],
        1017 => ["Primary revision number must be greater than 0."],
        1018 => ["Required element not defined."],

        1022 => ["%s should not have a value."],

        1025 => ["No value for %s."],

        1028 => ["This line is missing the double quote (\") at the end of the line. Adding one to continue processing."],
        1029 => ["\"CopyFiles\" directive should not copy files of type .inf to the system INF directory or any subdirectory of it."],
        1030 => ["\"CopyFiles\" directive should not copy files of type .cat."],
        1031 => ["\"DelFiles\" directive should not delete files of type .inf."],
        1032 => ["\"DelFiles\" directive should not delete files of type .cat."],

        1034 => ["Catalog files should not be listed in SourceDisksFiles."],

        1039 => ["No manufacturer defined in section [%s]."],

        1041 => ["Last value is empty, remove trailing commas."],
        1042 => ["Section listed twice in DestinationDirs. %s"],
        1043 => ["DirID value is missing."],
        1044 => ["Wrong value for DirID."],

        1047 => ["String tokens should be in quotes (and not blank) or should be numeric."],
        1048 => ["String entry (%s) already defined."],

        1050 => ["RunOnce entries require RunDLL32.exe."],

        1053 => ["[VERSION] must contain a CLASS directive."],
        1054 => ["[VERSION] must contain a CLASSGUID directive. Unable to validate CLASS directive."],
        1055 => ["ClassGUID should be in the format {nnnnnnn-nnnn-nnnn-nnnn-nnnnnnnnnnnn}"],
        1056 => ["ClassGUID cannot be a reference to a string. It must be listed directly in the [VERSION] section."],

        1058 => ["Directive: %s required in section [%s]"],
        1059 => ["Device class %s is either obsolete or reserved for system use."],
        1060 => ["No value for %s"],
        1061 => ["Value for EventLogType should be 'System', 'Application', or 'Security'"],

        1063 => ["Invalid RegRoot Value: %s"],
        1064 => ["CatalogFile name should be like 'filename.cat' (8.3 format)."],
        1065 => ["A service cannot use both SERVICE_WIN32 and SERVICE_DRIVER."],
        1066 => ["Bad value for %s in %s."],
        1067 => ["DriverVer date is in the future %s"],
        1068 => ["Illegal Value for %s"],
        1069 => ["Year must be in four digit format"],

        1071 => ["Bad value %s for %s. Consult the DDK Documentation."],
        1072 => ["%s is not a defined model."],

        1078 => ["Description length is too long. Limit is 1024 characters."],
        1079 => ["Directive: %s required (and must not be blank) in section [%s]"],
        1080 => ["Directive: %s required (and must not be blank) in section [%s]"],
        1081 => ["Directive: %s required (and must not be blank) in section [%s] for WHQL digital signature."],
        1082 => ["Date (%s-%s-%s) is invalid"],

        1085 => ["%s is an invalid decoration for a %s section."],
        1086 => ["Bad value for %s in %s."],
        1087 => ["Value side of directive cannot be blank."],
        1088 => ["%s parameter for %s is not optional"],
        1089 => ["A disabled service cannot be installed (StartType=0x4)"],

        1092 => ["No destination directory has been specified for this section. Correct the [DestinationDirs] section."],
        1093 => ["The system will automatically copy the inf file, it should not be listed in a copyfiles section."],
        1095 => ["String token %s is not defined in this section."],

        1098 => ["%s cannot be an empty string."],
        1099 => ["%s is not a valid LanguageID."],

        1100 => ["Invalid Priority Value for ConfigPriority"],
        1101 => ["Invalid Config Type for ConfigPriority"],
        1102 => ["Invalid IRQ value for IRQconfig"],
        1103 => ["The hexadecimal integer value exceeds range"],
        1104 => ["IRQConfig not assigned according to DDK specification"],
        1105 => ["IOConfig not assigned according to DDK specification"],
        1106 => ["Invalid DMA value for DMAconfig"],
        1107 => ["DMAConfig not assigned according to DDK specification"],
        1108 => ["MEMConfig not assigned according to DDK specification"],

        1110 => ["File \"%s\" is not listed in the [SourceDisksFiles]."],

        1112 => ["Flag %s must be specified for this registry key."],
        1113 => ["The value for this key is expected to be numeric."],
        1114 => ["The value for this key is expected to be 0 or 1."],

        1116 => ["Invalid value (%s) for DeviceUpgradeUnsupported. Value must be numeric."],
        1117 => ["Correct format for DeviceUpgradeUnsupported directive is 'DeviceUpgradeUnsupported=<value>'."],
        1118 => ["Invalid value (%s) for SysSetupPnpFlags."],
        1119 => ["Correct format for SysSetupPnpFlags directive is 'SysSetupPnpFlags=<value>'."],
        1120 => ["String token \"%s\" not defined."],

        1198 => ["INFs should use DOS-style end-of-line (&lt;cr&gt;&lt;lf&gt;). This INF is using some other format."],

        1301 => ["Security descriptors must be enclosed in quotation marks."],
        1302 => ["Filter drivers cannot be the controlling service (SPSVCINST_ASSOCSERVICE)."],
        1303 => ["\"RenFiles\" directive should not rename files of type .inf."],
        1304 => ["\"RenFiles\" directive should not rename files of type .cat."],
        1305 => ["Registry keys under HLKM\\SYSTEM\\CurrentControlSet\\Services must be set using an HKR abbreviation in an add-registry-section entry invoked from a service-install-section."],
        1306 => ["Files cannot be listed in both a CopyFiles and DelFiles or RenFiles section unless COPYFLG_NOPRUNE (0x00002000) is set."],
        1307 => ["INF Class must be less than 32 characters in length."],
        1308 => ["Inf lines should not be > x chars"],
        1309 => ["No DefaultDestDirs specified. Correct the [DestinationDirs] section."],
        
        1312 => ["INF files can not set registry entries under 'HKLM,System' unless the value is also under 'HKLM,System\\CurrentControlSet'."],
        1313 => ["INF files can not set registry entries under 'HKLM,System\\CurrentControlSet\\Enum'."],
        1314 => ["Month must be in two digit format."],
        1315 => ["Day must be in two digit format."],
        1316 => ["Year must be in four digit format."],
        1317 => ["Manufacturer-identifier is not unique in the [Manufacturer] section"],
        1318 => ["Invalid security descriptor string"],
        1319 => ["Error executing chksd.exe.  Please verify chksd.exe is in the directory that contains chkinf.bat"],
        1320 => ["Ini2Reg Directives are no longer allowed in device installation infs"],
        1321 => ["Unrecognized directive in WMI Security Section"],
        1322 => ["Malformed WMIInterface value: "],
        1323 => ["Malformed GUID in WMIInterface value"],
        1324 => ["Illegal flags in WMIInterface value"],
        1325 => ["Missing section name in WMIInterface value"],
        1326 => ["Invalid directive.  Directive must be set to SECURITY="],
        1327 => ["Only 1 security directive allowed in this section"],
        1328 => ["Referenced section not found"],
        1329 => ["Only usb hub drivers (pnp ID of the form USB\\CLASS_09) are allowed to reference StandardHub.Dev.NT in needs directive"],

        1401 => ["Relative paths are not allowed while specifying filenames"],
        1402 => ["Absolute paths are not allowed while specifying filenames"],
        1403 => ["UNC paths are not allowed while specifying filenames"],

        1500 => ["%s has already been added for Coinstallers32 registry key"],        
        1501 => ["INF should add %s as a service "],
        1502 => ["Value for directive %s should be set to %s"],
        1503 => ["%s should be in the form '%s'"],
        1504 => ["Malformed GUID specified in DriverCLSID value"],
        1505 => ["Reg-root %s should be specified for '%s' registry key."],
        1506 => ["Flag %s must be specified for '%s' registry key."],
        1507 => ["Invalid directive %s for [%s]"],
        1508 => ["Duplicate instance found for the %s directive"],        
        1509 => ["Illegal value for directive %s"],
        1510 => ["Service name '%s' has not been defined using the UmdfService directive"],
        1511 => ["Service '%s' has not been specified in the UmdfServiceOrder directive"],        
        1512 => ["WdfCoinstaller Entry Point is identified wrongly as '%s', it should be '%s'"],
        1513 => ["%s version %s should be added for HKR Coinstallers32 registry key"],
        1514 => ["%s file name should have the format: '%s' "],
        1515 => ["%s : %s is a copy-paste from WDF Samples"],        
        1516 => ["UMDF Driver(s) must request to use one of the Co-installers, either WudfCoinstaller.dll or WudfUpdate_MMNNN.dll"],
        1517 => ["Directive: '%s' is supported by only %s version %s"],       
        
        9999 => ["I don't know how to process this line"]
    );

    if (defined $ErrorTable{$ErrorNum}) {
        return($ErrorTable{$ErrorNum});
    } else {
        return($ErrorTable{9999});
    }
}

#-------------------------------------------------------------------------------------------------#
# this sub extracts a list of elements separated by ',' from the given line. It strips leading and#
# ending whitespaces. It flags error for whitespaces in elements.                                 #
#                                                                                                 #
# This is slightly more complicated. you can just use split function call to accomplish the same  #
# without the whitespace elimination.                                                             #
#-- GetList($line, $line_num, $check_empty) ------------------------------------------------------#
# Because we don't know if SplitLine was called to generate the value we're passed
# we have redundant swapping out of strings here =-(
sub GetList {

    my($line, $Lnum, $check_empty,$IgnoreUnmatchedNumberTokens) = @_;
    my(@list, $element);

    my ($cnt, $quote);
    my $QuoteNum = 0;
    my %AllQuotes;
    my ($Token, $QMToken);

    # Count the number of ", and tack on extra " if we have an odd number;
    # otherwise perl faults!
    $cnt = $line=~tr/"/"/;
    if ($cnt % 2) {
        # check if this odd quote is after a semicolon.
        if ($line =~ /(.(".*")*)*;.*"(.*".*")*/) {
            AddWarnRef(2008, $Lnum);
        } else {
            AddErrorRef(1028, $Lnum);
        }
        $line = "$line\"";
    }

    # Process compound quoted strings one-at-a-time, saving the quoted string
    # when we find it and doing some further processing to replace "" by ", and
    # to remove the enclosing quotes.
    my $i;
    my $cChar;
    my $pChar = " ";
    my $depth = 0;
    my @begin;
    my @end;
    my $inQuote = FALSE;

    for ($i = 0 ; $i <= length($line) ; $i++) {

        $cChar = substr($line,$i,1);

        if ($cChar eq '"') {
            if ($inQuote==TRUE) {
                if (substr($line,$i+1,1) eq '"') {
                    $i++;
                } else {
                    $end[$depth-1]     = $i;
                    $inQuote         = FALSE;
                }
            } else {
                $inQuote       = TRUE;
                $begin[$depth] = $i;
                $depth++;
            }
        }
    }

    for ($i = $#begin; $i >= 0; $i--) {
        $quote = substr($line, $begin[$i], ($end[$i] - $begin[$i]) +1);
        $AllQuotes{$QuoteNum}=$quote;
        substr($line, $begin[$i], ($end[$i] - $begin[$i]) + 1 )
            = "__QUOTED_STR_($QuoteNum)__";
        $QuoteNum++;
    }

    $line = RemoveStringTokens($Lnum, $line, $IgnoreUnmatchedNumberTokens);

    if (!defined($check_empty)) {
        $check_empty    = FALSE;
    }
    if ($check_empty) {
        AddErrorRef(1041, $Lnum) if ($line =~ /,$/);
    }

    @list = split(',', $line);
    foreach $element (@list) {

        while ($element =~ /__QUOTED_STR_\(([0-9]*)\)__/) {
            $AllQuotes{$1} = " " if (! defined $AllQuotes{$1});
            $element =~ s/__QUOTED_STR_\(([0-9]*)\)__/\"$AllQuotes{$1}\"/;
        }

        $element =~ s/^[\s\t]*//; # remove blanks at ends.
        $element =~ s/[\s\t]*$//; # remove blanks at ends.
    }

    return(@list);  #return value.
}

#-------------------------------------------------------------------------------------------------#
#-- GetWarning($WarnNum) -------------------------------------------------------------------------#
sub GetWarning {
    my($WarnNum) = $_[0];

    my(%WarningTable) = (
        2003 => ["Section [%s] not defined."],
        2004 => ["Service %s already defined in section [%s]."],
        2005 => ["Absolute paths shouldn't be used to service binaries."],

        2008 => ["Line has an odd number of \"s. Correct this to avoid erroneous processing."],
        2009 => ["TAB inside quoted string."],

        2011 => ["White spaces in identifier \"%s\"- should be quoted."],

        2014 => ["String tokens inside quotes are interpreted literally."],

        2018 => ["A class specific check module does not exist for this device class. Defaulting to generic module."],

        2020 => ["Directive %s should be defined in section %s"],
        2021 => ["%%'s in string tokens are interpreted literally."],
        2023 => ["Use a string token, and put localizable text in the [Strings] section."],
        2024 => ["Use of the RunOnce Registry key is not recommended."],
        2025 => ["No value for %s."],
        2026 => ["An INF should not copy itself."],
        2027 => ["StartType of %d %s"],
        2028 => ["\"CopyFiles\" directive should not copy files of type .inf."],
        2029 => ["String %s not referenced."],

        2033 => ["INF files should not be listed in SourceDisksFiles."],
        2034 => ["No destination dir was defined for this section. Your driver is being copied to the DefaultDestDir location specified (%s)."],
        2035 => ["Drivers should be copied to the drivers directory (DirID 12). Check your [DestinationDirs] section."],

        2077 => ["System defined classes should usually not specify a class installer."],

        2083 => ["Section [%s] not referenced"],

        2100 => ["No undecorated [STRINGS] section found. Using [%s] as the default instead."],
        2101 => ["Only system critical devices/drivers should set ErrorControl to %d"],

        2111 => ["[SourceDisksFiles] section not defined - full CopyFiles checking not done."],
        2112 => ["File \"%s\" is not listed in the [SourceDisksFiles]."],

        2147 => ["Section (%s) not referenced by any copyfiles, renfiles or delfiles keywords."],

        2190 => ["A driver service should not use SERVICE_INTERACTIVE_PROCESS."],
        2191 => ["Empty DDInstall section will not install any drivers."],
        2192 => ["Leading back-slashes in path names refer to the root of the source media not root of physical drive."],

        2201 => ["This coinstaller will affect installation for all devices of this class."],
        2202 => ["Unrecognized directive: %s"],
        2203 => ["Custom defined Classes should have a [ClassInstall32] section."],
        2204 => ["New-File-Name and Old-File-Name should be different."],
        2205 => ["INFs should use DOS-style end-of-line (&lt;cr&gt;&lt;lf&gt;). This INF is using some other format."],
        2206 => ["SourceDisksNames section defined but no CopyFiles directive found."],
        2207 => ["SourceDisksFiles section defined but no CopyFiles directive found."],
        2208 => ["NT-specific section(s) found.  Ignoring general section."],
        2209 => ["String %s is not defined in the default [Strings] section."],
        2210 => ["\"DefaultInstall\" is not typically used for a DDInstall section. Recommend using a different name."],
        2211 => ["Directive: %s expected in section [%s]"],
        2212 => ["No Copyright information found."],
        2213 => ["INF files should not set registry entries under 'HKLM,System\\CurrentControlSet\\Control\\Class'."],
        2214 => ["INF files should not set registry entries under 'HKLM,System\\CurrentControlSet\\Hardware Profiles'."],
        
        2215 => ["Class %s (ClassGUID %s) is unrecognized."],
        
        2237 => ["INF files should not set registry entries under 'HKLM,System\\CurrentControlSet\\Control\\CriticalDeviceDatabase'."],

        2300 => ["Service binary should be copied and run from %%windir\%\\system32\\drivers\\UMDF directory (Use %%12\%\\UMDF)."],
        2301 => ["Duplicate instance found for '%s' registry entry"],
        2302 => ["%s section doesn't install device-specific Coinstallers using the 'CoInstallers32' registry key."], 
        2303 => ["KMDF Driver is not using any WDF Co-installers. If the required version or a higher version of the framework is not present or functioning on the system, this driver package will fail to work"],
        2304 => ["KMDF Driver is not using any WDF Co-installers. This section is also intended for Win2K and below architectures. If the required version or a higher version of the framework is not present or functioning on the system, this driver package will fail to work"],
                    
       9999 => ["The above line was looked at"]
    );
    if (defined $WarningTable{$WarnNum}) {
        return($WarningTable{$WarnNum});
    } else {
        return($WarningTable{9999});
    }
}

#-------------------------------------------------------------------------------------------------#
# Prepares a global hash with a list of all strings in the file                                   #
#-- LearnStrings($SectionName) -------------------------------------------------------------------#
sub LearnStrings {

    if ($DEBUG) {
        print("Beginning LearnStrings...\n");
    }

    my($line);
    my($Section)    = shift;
    my($count)      =    0;
    my(@lines)      = $CurrentINF->getOrigSection($Section);
    my($String, $StringValue, $QMString);

    my($MaxLength) = 1024;

    # Easiest way to test for string usage is to open the entire file
    # as an array- this way we don't need to add logic to process
    # every known section object.
    # Eventually, we may want to add a function to InfFile.pm to return
    # the entire, modified, INF
    foreach $line (@lines) {

        print "$count: $line\n" if ($DEBUG);
        ($count++, next) if ($line =~ /^[\s\t]*$/); # Skip if it's a blank line

        # Start with the INF line
        ($String,$StringValue) = SplitLine($Section, $count, $line);

        $QMString  = uc(quotemeta($String));
        if (! defined($StringValue) ) {
            # add an error
            next;
        }

        AddErrorRef(1005, $CurrentINF->getIndex($Section, $count), $MaxLength)
            if (length($StringValue) > $MaxLength);

        if (defined($StringVals{$QMString})) {
            AddErrorRef(1048, $CurrentINF->getIndex($Section, $count), "$String" ); # String entry redefined.
            $count++;
            next;
        }

        # Verify $StringValue is in quotes or is a numeric value
        if ($StringValue !~ /^\".+\"$/) {
            if ( $StringValue !~ /^(0x)?\d*$/i ) {
                AddErrorRef(1047, $CurrentINF->getIndex($Section,$count));
            }
        }

                if ($StringValue =~ /^\"(.*)\"$/) {
                        $StringValue = $1;
                }

        $StringVals{$QMString} = $StringValue; #Store the value for later use.

        # String definitions containing %.*% are taken literally
        if ($StringValue =~ /%(.*)%/) {
            AddWarnRef(2021, $CurrentINF->getIndex($Section,$count));
        }
        $count++;
    }

    $CheckedSections{uc($Section)} = TRUE;

    if ($DEBUG) {
        print("Leaving LearnStrings...\n");
    }
    return(TRUE);
}

#-------------------------------------------------------------------------------------------------#
# Writes %inffile%.htm and, optionally, NTLOG data
#-- PrintResults($OutFile, $InfFile) -------------------------------------------------------------#
sub PrintResults {
    my($OutFile)    =   $_[0];
    my($InfFile)    =   $_[1];
    my($IN, $OUT, $NTLOG)   =   "";
    my($err, @errs);
    my($lp, $count);
    my $LogFile;

    my($DataFile)   =  $CurrentINF->datafilename(); # same as InfFile if ANSI inf, UTF8 data if UNICODE inf

    open(IN,  "<$DataFile") ||
        (warn("Couldn't open $DataFile: $!\nAborting this inf\n"),return(FALSE));
    open(OUT, ">$OutFile") ||
        (warn("Couldn't open $OutFile: $!\nAborting this inf\n"),return(FALSE));

    if (defined $Globals{"LOGFILE"} ) {
        $LogFile = $Globals{"LOGFILE"};
        open(NTLOG, ">>$LogFile") || die("Couldn't open $LogFile: $!\n");
        print(NTLOG "$InfFile: ") if ( defined $Globals{"LOGFILE"} );
        ($ErrorCount == 0) ?    print(NTLOG "PASSED\n") : print(NTLOG "FAILED\n");
        print( NTLOG "NTLOG REPORT--------------\n");
        printf(NTLOG "Total Lines: %10i  |\n", $TotalLines);
        printf(NTLOG "Total Errors: %9i  |\n", $ErrorCount);
        printf(NTLOG "Total Warnings: %7i  |\n", $WarnCount);
        print( NTLOG "--------------------------\n");
    }

    print(OUT "<HTML><HEAD>\n");
    if(uc $DataFile ne uc $InfFile) {
       # we're using a temporary utf8 file
       # so make sure html file is marked as utf8
       print(OUT "<meta HTTP-EQUIV=\"Content-Type\" CONTENT=\"text/html; charset=utf-8\">\n");
    }
    print(OUT "<meta NAME=\"GENERATOR\" CONTENT=\"ChkInf\">\n");
    print(OUT "<TITLE>Summary of ChkInf results for $InfFile</TITLE></HEAD>\n");
    print(OUT "<BODY VLINK=#000000>\n");

    print(OUT "<H1>Summary of \"<FONT COLOR=#990099>$InfFile</FONT>\"</H1>\n");
    print(OUT "<TABLE CELLSPACING=0 CELLPADDING=0 BORDER=0>\n");

    if ($ErrorCount >= 1) {
       print(OUT "\t<TR><TD><H3><A HREF=\"#ERRORS\">Total Errors:</A></TD>\n");
       print(OUT "\t    <TD>&nbsp;&nbsp;&nbsp;&nbsp;</TD>\n");
       print(OUT "\t    <TD ALIGN=RIGHT><H3><FONT COLOR=RED>$ErrorCount</FONT></TD></TR>\n");
    }

    if ($WarnCount >= 1) {
        print(OUT "\t<TR><TD><H3><A HREF=\"#WARNINGS\">Total Warnings:</A></TD>\n");
        print(OUT "\t    <TD>&nbsp;&nbsp;&nbsp;&nbsp;</TD>\n");
        print(OUT "\t    <TD ALIGN=RIGHT><H3><FONT COLOR=BLUE>$WarnCount</FONT></TD></TR>\n");
    }

    if (($ErrorCount == 0) and ($WarnCount == 0)) {
       print(OUT "\t<TR><TD><H3><FONT COLOR=#990099><B><I>INF Passed with no warnings or errors!");
       print(OUT "</I></B></FONT></H3></TD></TR>\n");
    }
    print(OUT "</TABLE><BR>\n");

    print(OUT "\t<HR>\n") if ($ErrorCount >= 1);
    print(OUT "\t<FONT COLOR=RED><H3><A NAME=\"ERRORS\">Errors:</A></H3>\n")
        if ($ErrorCount >= 1);
    foreach $lp ( sort(NUMERIC keys(%ErrorTable)) ) {
        if (defined ($err = $ErrorTable{$lp})) {
            @errs = split ('; ', $err);
            foreach $err (@errs) {
                next if ($err=~ /^$/);
                $err =~ s/>/&gt;/;
                $err =~ s/</&lt;/;
                print(OUT   "\t\t<LI><A HREF=\"#Lnum$lp\">Line $lp</A>: $err \n");
                print(NTLOG "Line $lp: ERROR: $err \n") if (defined $Globals{"LOGFILE"});
            }
        }
    }

    print(OUT "\t</FONT>\n");

    print(OUT "\t<HR>\n") if ($WarnCount >= 1);
    print(OUT "\t<FONT COLOR=BLUE><H3><A NAME=\"WARNINGS\">Warnings:</A></H3>\n")
        if ($WarnCount >= 1);
    foreach $lp ( sort(NUMERIC keys(%WarnTable)) ) {
        if (defined ($err = $WarnTable{$lp})) {
            @errs = split ('; ', $err);
            foreach $err (@errs) {
                next if ($err=~ /^$/);
                print(OUT   "\t\t<LI><A HREF=\"#Lnum$lp\">Line $lp</A>: $err \n");
                print(NTLOG "Line $lp: WARNING: $err \n") if (defined $Globals{"LOGFILE"});
            }
        }
    }
    print(OUT "\t</FONT>\n");

    if (defined($Module)) {
        $Module->PrintHeader();
    }

    print(OUT "\t<BR><HR>\n<H3>Annotated INF:</H3>\n<PRE>\n");
    # Handle line 0 errors & warnings
    print(OUT "<A NAME=\"Lnum0\">\n");

    if (defined ($err = $ErrorTable{0})) {
        print(OUT "<FONT COLOR=RED>");
        @errs = split ('; ', $err);
        foreach $err (@errs) {
            next if ($err=~ /^$/);
            print(OUT "; $err \n");
        }
        print(OUT "</FONT>");
    }

    if (defined ($err = $WarnTable{0})) {
        print(OUT "<FONT COLOR=BLUE>");
        @errs = split ('; ', $err);
        foreach $err (@errs) {
            next if ($err=~ /^$/);
            print(OUT "; $err \n");
        }
        print(OUT "</FONT>");
    }

    while(<IN>) {
        print(OUT "<A NAME=\"Lnum", $., "\">");
        chop if /\n$/;           # removes \n
        s/</&lt\;/g;             # so that htm interprets them right
        s/>/&gt\;/g;
        print(OUT);
        print(OUT "</A>\n");

        if (defined ($err = $ErrorTable{$.})) {
            print(OUT "<FONT COLOR=RED>");
            @errs = split ('; ', $err);
            foreach $err (@errs) {
                next if ($err=~ /^$/);
                print(OUT "; $err \n");
            }
            print(OUT "</FONT>");
        }

        if (defined ($err = $WarnTable{$.})) {
            print(OUT "<FONT COLOR=BLUE>");
            @errs = split ('; ', $err);
            foreach $err (@errs) {
                next if ($err=~ /^$/);
                print(OUT "; $err \n");
            }
            print(OUT "</FONT>");
        }
    }

    $lp = $.; # Last line of file
    $lp++;

    print(OUT "<A NAME=\"Lnum", $lp, "\">"); # Print out any warnings or errors
    print(OUT "</A>\n");                     # that have been flagged for the end
                                             # of the file.
    if (defined ($err = $ErrorTable{$lp})) { # Putting them on the actual last line
        print(OUT "<FONT COLOR=RED>");       # can make output very ugly if another
        @errs = split ('; ', $err);          # error or warning is also flagged
        foreach $err (@errs) {               # for that line by a later subroutine!
            next if ($err=~ /^$/);
            print(OUT "; ERROR: $err \n");
        }
        print(OUT "</FONT>");
    }
    if (defined ($err = $WarnTable{$lp})) {
        print(OUT "<FONT COLOR=BLUE>");
        @errs = split ('; ', $err);
        foreach $err (@errs) {
            next if ($err=~ /^$/);
            print(OUT "; $err \n");
        }
        print(OUT "</FONT>");
    }

    $lp++;    # Step past it by one

    print(OUT "<A NAME=\"Lnum", $lp, "\">"); # Print out any warnings or errors
    print(OUT "</A>\n");                     # that have been flagged for the end
                                             # of the file.
    if (defined ($err = $ErrorTable{$lp})) { # Putting them on the actual last line
        print(OUT "<FONT COLOR=RED>");       # can make output very ugly if another
        @errs = split ('; ', $err);          # error or warning is also flagged
        foreach $err (@errs) {               # for that line by a later subroutine!
            next if ($err=~ /^$/);
            print(OUT "; ERROR: $err \n");
        }
        print(OUT "</FONT>");
    }
    if (defined ($err = $WarnTable{$lp})) {
        print(OUT "<FONT COLOR=BLUE>");
        @errs = split ('; ', $err);
        foreach $err (@errs) {
            next if ($err=~ /^$/);
            print(OUT "; $err \n");
        }
        print(OUT "</FONT>");
    }

    print(OUT "</PRE>\n");
    close(IN);

    print(OUT "</PRE>\n");
    print(NTLOG "\n\n") if ( defined $Globals{"LOGFILE"} );
    close(NTLOG) if ( defined $Globals{"LOGFILE"} );
    close(OUT);

    $FilesChecked{$InfFile} = "$WarnCount:$ErrorCount:$OutFile";

}

#-------------------------------------------------------------------------------------------------#
# Writes summary.htm which lists the error/warning counts for each INF and links to the INF report
#-- PrintSummary() -------------------------------------------------------------------------------#
sub PrintSummary {

    my($File,@Values,$Link);
    my(@Keys) = sort(keys(%FilesChecked));
    my($TotalErrors)  =  0;
    my($TotalWarnings)  =  0;
    open(OUT, ">$_[0]") || (warn("Couldn't open $_[0]: $!\nAborting summary.htm"),return(255));

    print(OUT "<HTML><HEAD><TITLE>Summary of ChkInf Results</TITLE></HEAD>");
    print(OUT "<BODY LINK=#000000 VLINK=#000000>\n");
    print(OUT "\tChkInf " . Version() . ": &copy; Copyright " . CopyrightDates() . " Microsoft Corp.  All rights reserved.<BR>\n");
    print(OUT "\t<HR SIZE=10>\n");
    print(OUT "\t<H4>Files processed: ");
    print(OUT (@Keys+0));
    print(OUT "</H4>\n\t<BR>\n");
    print(OUT "\t<TABLE CELLPADDING=3 BORDER=0>\n");
    foreach $File (@Keys) {
        @Values = split(/:/,$FilesChecked{$File});
        $Values[2] =~ s/^\.\\htm\\//;
        print(OUT "\t\t<TR><TD><FONT SIZE=+1><TT><A HREF=\"$Values[2]\">$File:</A></TT></FONT></TD>\n");
        print(OUT "\t\t<TD><FONT COLOR=#FF0000>Errors  : $Values[1]</FONT></TD></TR>\n");
        print(OUT "\t\t<TR><TD></TD><TD><FONT COLOR=#0000FF>Warnings: $Values[0]</FONT></TD></TR>\n");
        print(OUT "\t\t<TR><TD ROWSPAN=2><FONT COLOR=#FFFFFF>&nbsp;&nbsp;&nbsp;</FONT></TD></TR>\n");
        print(OUT "\t\t<TR><TD> </TD></TR>\n\n");
        $TotalErrors += $Values[1] if (defined($Values[1]));
        $TotalWarnings += $Values[0] if (defined($Values[0]));
    }
    print(OUT "\t</TABLE>\n");
    print(OUT "</BODY></HTML>\n");

    close(OUT);

    if ($TotalErrors >= 1) {
       return (-1);
    }
    elsif ($TotalWarnings >= 1) {
       return (-2);
    }
    else {
       return(0);
    }
}

#-------------------------------------------------------------------------------------------------#
# Legacy routine
#-- QuotedString() -------------------------------------------------------------------------------#
sub QuotedString {  return();   } # Needs to be defined to not break old modules.


#-------------------------------------------------------------------------------------------------#
# Replaces string tokens w/ their literals
#-- RemoveStringTokens($linenum, $linetxt) -------------------------------------------------------#
sub RemoveStringTokens {
    my $linenum = shift;
    my $linetxt = shift;
    my $IgnoreUnmatchedNumberTokens = shift;
    my $i;
    my ($start_pos, $end_pos);
    my $this_token;
    my %replacements=();

        #
        # Check each char in the line for the start of a string token
        #
        for ($i=0; $i < length($linetxt); $i++ ) {
                #
                # If the character is the start of a string token, find
                # the end of the token.
                #
                if ( substr($linetxt,$i,1) eq "%" ) {
                        #
                        # Mark the start of the token and move to the
                        # next character position.
                        #
                        $start_pos=$i++;

                        #
                        # The current character isn't the last
                        # character in the line.
                        #
                        if ( $i >= length($linetxt) ) {
                                next;
                        }

                        #
                        # Is this character the end of the string or line?
                        #
                        while ( (substr($linetxt,$i,1) ne "%" ) and
                              ($i  < length($linetxt)       ) ) {
                                #
                                # Not the end of the line, so increment
                                # the current position.
                                #
                                $i++;
                        }

                        #
                        # We reached the end of the line before the token ended,
                        # so treat it as an empty token.
                        #
                        if ($i  >= length($linetxt) )  {
                           #
                           # Check to see if we should ignore numbers after a single %
                           # as in the case of IoConfig and MemConfig properties
                           #
                            if ( (defined($IgnoreUnmatchedNumberTokens)) and ($IgnoreUnmatchedNumberTokens) ) {
                               my $PossibleNumber = substr($linetxt,$start_pos+1);
                               if ( $PossibleNumber =~ /^[0-9a-fA-F]+/ ) {
                                  next;
                               }
                            }
                            $end_pos = ($start_pos + 1);
                        }

                        #
                        # To get here, we had to find the end of the token so mark it.
                        #
                        $end_pos = $i;

                        #
                        # Make sure we didn't find an escaped % (%%)
                        #
                        if ( $end_pos == ($start_pos + 1) ) {
                                $replacements{$start_pos}=[($end_pos - $start_pos)+1,"%"];
                                next;
                        }

                        #
                        # Find the token, which is stuffed between % and %.  Start and end
                        # positions must take into account the fact that we don't want the %'s
                        #
                        $this_token=quotemeta(substr($linetxt, $start_pos+1, ($end_pos - $start_pos)-1));

                        #
                        # Make sure we have a definition for this token.
                        #
                        if (defined $StringVals{uc($this_token)}) {
                                #
                                # Make a record of the start, end, and replacement for this token.
                                #
                                $replacements{$start_pos}=[($end_pos - $start_pos)+1,$StringVals{uc($this_token)}];
                        }
                        elsif ( $this_token =~ /\A[0-9\-]+\Z/ ) {
                                next; #ignore DirIDs
                        } else {
                                #
                                # Add a error for the undefined token.
                                #
                                AddErrorRef(1120, $linenum, $this_token);
                        }
                }
        }

        #
        # Now that we have the tokens, replace them in reverse order to that we don't screw up
        # our index numbers.
        #
        my $index;
        foreach $index (reverse sort { $a <=> $b } keys(%replacements) ) {
                substr($linetxt, $index, ${$replacements{$index}}[0], ${$replacements{$index}}[1]);
        }

        return($linetxt);
}

#-------------------------------------------------------------------------------------------------#
# Clears all global variables in order to start a new inf                                         #
#-- ResetGlobals($inffile, $DCOptions, [$IncInfDir]) ---------------------------------------------#
sub ResetGlobals {
    shift;  # first param is InfFileName
    # Auto-include system defined strings
    %StringVals      = (
                        "55","PrintProcDir",
                        "54","\$OSLOADER_DIR\$",
                        "53","UserProfileDir",
                        "52","SpoolDriversDir",
                        "51","SpoolDir",
                        "50","\$WinDir\$\\system",
                        "30","\$BootDrive\$",
                        "25","SharedDir",
                        "24","AppsDir",
                        "23","ICMDir",
                        "21","ViewersDir",
                        "20","\$FontsDir\$",
                        "18","HelpDir",
                        "17","\$InfDir\$",
                        "12","\$WinDir\$\\system32\\drivers",
                        "11","\$WinDir\$\\system32",
                        "10","\$Windir\$",
                        "01","\$InfSrcDriveAndPath\$",
                        "16406","All Users\\Start Menu",
                        "16407", "All Users\Start Menu\\Programs",
                        "16408", "All Users\Start Menu\\Programs\Startup",
                        "16409", "All Users\\Desktop",
                        "16415", "All Users\\Favorites",
                        "16419", "All Users\\Application Data",
                        "16422", "Program Files",
                        "16425", "\$WinDir\$\\system32", # on RISC
                        "16426", "Program Files", # on RISC
                        "16427", "Program Files\\Common",
                        "16428", "Program Files\\Common", # on RISC
                        "16429", "All Users\\Templates ",
                        "16430", "All Users\\Documents ",
                        "-1","AbsPath");

    $DCOptions   = shift;
    $INTERNAL    = shift;

    %ErrorTable = ();
    $ErrorCount =  0;

    %WarnTable = ();
    $WarnCount =  0;

    %CheckedSections = ();

    @CachedSDFiles = ();

    # Set/Clear some globals
    $Globals{"MS_INTERNAL"}         = $INTERNAL; # make visible to packages (eg: Printer.pm) via GetGlobalValue() subroutine.
    $Globals{"AllSISec"}            = {};
    $Globals{"AllSINames"}          = {};
    $Globals{"AllSINameBins"}       = {};
    $Globals{"LOWERFILTERS"}        = [];
    $Globals{"UPPERFILTERS"}        = [];
    $Globals{"ServiceSectionFlags"} = [];
    $Globals{"Platform"}            = "";
    $Globals{"ModelSection"} ="";
    $OSContext = " ";
    %ServiceInstallBin  = ();
    
    return(1);
}

#-------------------------------------------------------------------------------------------------#
# Takes a line, resolves quoted strings, splits it along the /($1)=($2)/ and returns the two      #
# pieces. If the line lacks a /=/, the whole line is returned as $1 with $2 being set equal       #
# to ""                                                                                           #
# ($Directive, $Value) SplitLine($SectionName, $SectionOffset, $Line) ----------------------------#
sub SplitLine {

    if ($DEBUG) {
        print "Beginning SplitLine...\n";
    }

    return() unless defined($_[2]);
    my $Section  = shift;
    my $Offset   = shift;
    my $Line     = shift;
    my $IgnoreUnmatchedNumberTokens = shift;
    my $Lnum     = $CurrentINF->getIndex($Section,$Offset);

    my($Directive, $Value, $cnt, $quote);
    my $QuoteNum = 0;
    my %AllQuotes;

    # Count the number of ", and tack on extra " if we have an odd number;
    # otherwise perl faults!
    $cnt = $Line=~tr/"/"/;
    if ($cnt % 2) {
        # check if this odd quote is after a semicolon.
        if ($Line =~ /(.(".*")*)*;.*"(.*".*")*/) {
            AddWarnRef(2008, $Lnum);
        } else {
            AddErrorRef(1028, $Lnum);
        }
        $Line = "$Line\"";
    }

    # Process compound quoted strings one-at-a-time, saving the quoted string
    # when we find it and doing some further processing to replace "" by ", and
    # to remove the enclosing quotes.

        # Handle double (i.e. escaped) quotes
      while ( $Line=~s/("{2}[^"]*"{2})"/__QUOTED_STR_($QuoteNum)__/ ) {
        $quote = $&;
        $quote =~ s/^"(.*)"$/$1/;
        $quote =~ s/""/"/g;
        # Test for TABs inside quoted strings here
        AddWarnRef(2009, $Lnum) if ($quote =~ /\t/);
        $AllQuotes{$QuoteNum}=$quote;
        AddWarnRef(2014, $Lnum) if ($quote =~ /%.+%/);
        $QuoteNum++;
        print "$Line\n" if ($DEBUG);
    }

        # Now handle single quotes
    while ( $Line=~s/"("*[^"]*"*)"/__QUOTED_STR_($QuoteNum)__/ ) {
        $quote = $&;
        $quote =~ s/^"(.*)"$/$1/;
        $quote =~ s/""/"/g;
        # Test for TABs inside quoted strings here
        AddWarnRef(2009, $Lnum) if ($quote =~ /\t/);
        $AllQuotes{$QuoteNum}=$quote;
        AddWarnRef(2014, $Lnum) if ($quote =~ /%.+%/);
        $QuoteNum++;
        print "$Line\n" if ($DEBUG);
    }

    # Now that we've swapped out the quotes, get the values
    $Directive = GetDirective($Section, $Offset, $Line);
    $Value     = GetDirectiveValue($Section, $Offset, $Line);

    $Directive =~ s/^[\s\t]*//;
    $Directive =~ s/[\s\t]*$//;
    $Value     =~ s/^[\s\t]*//;
    $Value     =~ s/[\s\t]*$//;

    $Directive = RemoveStringTokens($CurrentINF->getIndex($Section, $Offset), $Directive);

    $Value     = RemoveStringTokens($CurrentINF->getIndex($Section, $Offset), $Value, $IgnoreUnmatchedNumberTokens);

    # Swap the strings back in before returning including a single quote set
    while ($Directive =~ /__QUOTED_STR_\(([0-9]*)\)__/) {
        $Directive =~ s/__QUOTED_STR_\(([0-9]*)\)__/$AllQuotes{$1}/;
    }

    while ($Value =~ /__QUOTED_STR_\(([0-9]*)\)__/) {
        $Value =~ s/__QUOTED_STR_\(([0-9]*)\)__/\"$AllQuotes{$1}\"/;
    }

    if ($DEBUG) {
        print "Leaving SplitLine...\n";
    }
    return($Directive, $Value);
}

#-------------------------------------------------------------------------------------------------#
# Returns an array of the lines in the file.  No processing is done on those lines.
# Assumes Unicode files are already converted to UTF8 (true for INF file under test, 
# otherwise use InfFile::ConvertIfUnicodeINF to convert the file first [assumes .INF extension])
#
# ReadAllLines($FileName) ------------------------------------------------------------------------#
sub ReadAllLines {

        my $file = shift;

        open(fHndl,"$file") || return();
        my @lines = <fHndl>;
        close(fHndl);

        return(@lines);
}


#-------------------------------------------------------------------------------------------------#
sub CheckStringsWithLangID {
        my %OrigStrings;
        my %TheseStrings;
        my ($sec, $string);

    #  A language ID is a 16 bit value which is the combination of a
    #  primary language ID and a secondary language ID.  The bits are
    #  allocated as follows:
    #
    #       +-----------------------+-------------------------+
    #       |     Sublanguage ID    |   Primary Language ID   |
    #       +-----------------------+-------------------------+
    #        15                   10 9                       0   bit
    #
    # See %DDK_ROOT%\inc\winnt.h for more information
    my %ValidStringDecorations = ( # Pregenerated list of valid LanguageID values
    "0001"=>1,"0002"=>1,"0003"=>1,"0004"=>1,"0005"=>1,"0006"=>1,"0007"=>1,"0008"=>1,
    "0009"=>1,"000A"=>1,"000B"=>1,"000C"=>1,"000D"=>1,"000E"=>1,"000F"=>1,"0010"=>1,
    "0011"=>1,"0012"=>1,"0013"=>1,"0014"=>1,"0015"=>1,"0016"=>1,"0018"=>1,"0019"=>1,
    "001A"=>1,"001B"=>1,"001C"=>1,"001D"=>1,"001E"=>1,"001F"=>1,"0020"=>1,"0021"=>1,
    "0022"=>1,"0023"=>1,"0024"=>1,"0025"=>1,"0026"=>1,"0027"=>1,"0029"=>1,"002A"=>1,
    "002B"=>1,"002C"=>1,"002D"=>1,"002F"=>1,"0036"=>1,"0037"=>1,"0038"=>1,"0039"=>1,
    "003E"=>1,"003F"=>1,"0040"=>1,"0041"=>1,"0043"=>1,"0044"=>1,"0045"=>1,"0046"=>1,
    "0047"=>1,"0048"=>1,"0049"=>1,"004A"=>1,"004B"=>1,"004C"=>1,"004D"=>1,"004E"=>1,
    "004F"=>1,"0050"=>1,"0056"=>1,"0057"=>1,"0058"=>1,"0059"=>1,"005A"=>1,"0060"=>1,
    "0061"=>1,"0065"=>1,"0401"=>1,"0402"=>1,"0403"=>1,"0404"=>1,"0405"=>1,"0406"=>1,
    "0407"=>1,"0408"=>1,"0409"=>1,"040A"=>1,"040B"=>1,"040C"=>1,"040D"=>1,"040E"=>1,
    "040F"=>1,"0410"=>1,"0411"=>1,"0412"=>1,"0413"=>1,"0414"=>1,"0415"=>1,"0416"=>1,
    "0418"=>1,"0419"=>1,"041A"=>1,"041B"=>1,"041C"=>1,"041D"=>1,"041E"=>1,"041F"=>1,
    "0420"=>1,"0421"=>1,"0422"=>1,"0423"=>1,"0424"=>1,"0425"=>1,"0426"=>1,"0427"=>1,
    "0429"=>1,"042A"=>1,"042B"=>1,"042C"=>1,"042D"=>1,"042F"=>1,"0436"=>1,"0437"=>1,
    "0438"=>1,"0439"=>1,"043E"=>1,"043F"=>1,"0440"=>1,"0441"=>1,"0443"=>1,"0444"=>1,
    "0445"=>1,"0446"=>1,"0447"=>1,"0448"=>1,"0449"=>1,"044A"=>1,"044B"=>1,"044C"=>1,
    "044D"=>1,"044E"=>1,"044F"=>1,"0450"=>1,"0456"=>1,"0457"=>1,"0458"=>1,"0459"=>1,
    "045A"=>1,"0460"=>1,"0461"=>1,"0465"=>1,"0801"=>1,"0802"=>1,"0803"=>1,"0804"=>1,
    "0805"=>1,"0806"=>1,"0807"=>1,"0808"=>1,"0809"=>1,"080A"=>1,"080B"=>1,"080C"=>1,
    "080D"=>1,"080E"=>1,"080F"=>1,"0810"=>1,"0811"=>1,"0812"=>1,"0813"=>1,"0814"=>1,
    "0815"=>1,"0816"=>1,"0818"=>1,"0819"=>1,"081A"=>1,"081B"=>1,"081C"=>1,"081D"=>1,
    "081E"=>1,"081F"=>1,"0820"=>1,"0821"=>1,"0822"=>1,"0823"=>1,"0824"=>1,"0825"=>1,
    "0826"=>1,"0827"=>1,"0829"=>1,"082A"=>1,"082B"=>1,"082C"=>1,"082D"=>1,"082F"=>1,
    "0836"=>1,"0837"=>1,"0838"=>1,"0839"=>1,"083E"=>1,"083F"=>1,"0840"=>1,"0841"=>1,
    "0843"=>1,"0844"=>1,"0845"=>1,"0846"=>1,"0847"=>1,"0848"=>1,"0849"=>1,"084A"=>1,
    "084B"=>1,"084C"=>1,"084D"=>1,"084E"=>1,"084F"=>1,"0850"=>1,"0856"=>1,"0857"=>1,
    "0858"=>1,"0859"=>1,"085A"=>1,"0860"=>1,"0861"=>1,"0865"=>1,"0C01"=>1,"0C04"=>1,
    "0C07"=>1,"0C09"=>1,"0C0A"=>1,"0C0C"=>1,"0C1A"=>1,"1001"=>1,"1004"=>1,"1007"=>1,
    "1009"=>1,"100A"=>1,"100C"=>1,"1401"=>1,"1404"=>1,"1407"=>1,"1409"=>1,"140A"=>1,
    "140C"=>1,"1801"=>1,"1809"=>1,"180A"=>1,"180C"=>1,"1C01"=>1,"1C09"=>1,"1C0A"=>1,
    "2001"=>1,"2009"=>1,"200A"=>1,"2401"=>1,"2409"=>1,"240A"=>1,"2801"=>1,"2809"=>1,
    "280A"=>1,"2C01"=>1,"2C09"=>1,"2C0A"=>1,"3001"=>1,"3009"=>1,"300A"=>1,"3401"=>1,
    "3409"=>1,"340A"=>1,"3801"=>1,"380A"=>1,"3C01"=>1,"3C0A"=>1,"4001"=>1,"400A"=>1,
    "440A"=>1,"480A"=>1,"4C0A"=>1,"500A"=>1,"0000"=>1,"007F"=>1,"0400"=>1,"0800"=>1,
    );

    if ( $CurrentINF->sectionDefined("STRINGS") ) {
        %OrigStrings = GetStrings("STRINGS");
    }

    foreach $sec ( $CurrentINF->getSections() ) {
        if ($sec =~ m/strings\.([0-9A-F]{4})/i) {

            if (! %OrigStrings) {
                %OrigStrings = GetStrings($sec);
            }

            if (defined $ValidStringDecorations{uc $1}) {
                %TheseStrings = GetStrings($sec);
            } else {
               AddErrorRef(1099,$CurrentINF->getIndex($sec)-1, $1);
               $CheckedSections{$sec} = TRUE;
            }
            foreach $string (keys %OrigStrings) {
                AddErrorRef(1095,$CurrentINF->getIndex($sec)-1, $string)
                    unless (defined $TheseStrings{$string});
            }

            foreach $string (keys %TheseStrings) {
                AddWarnRef(2209,$CurrentINF->getIndex($sec,$TheseStrings{$string}), $string)
                    unless (defined $OrigStrings{$string});
            }
        }
    }
}

#-------------------------------------------------------------------------------------------------#
sub GetStrings {
    my($line);
    my($Section)    = shift;
    my($count)      =    0;
    my(@lines)      = $CurrentINF->getOrigSection($Section);
    my($String, $StringValue, %Strings);
    $CheckedSections{$Section} = TRUE;

    foreach $line (@lines) {
            ($String,$StringValue) = SplitLine($Section, $count, $line);
                ($count++,next) if (! defined $String or ! defined $StringValue);
            $Strings{$String} = $count unless ($String =~ /^\s*$/);
                $count++;
        }
        return(%Strings);
}

}

# Empty stubs for now
sub GetLine   {};

#-------------------------------------------------------------------------------------------------#
# Check for relative, absolute or UNC path in the given line.
#-------------------------------------------------------------------------------------------------#
sub IsRelativeAbsoluteOrUnc {
    
    my($returnval) = 0;
    my($line)   = shift;
    
    if( $line =~ /\.\./ ){
        $returnval = 1;
    }
    
    if( $line =~ /.:/ ){
        $returnval = 2;
    }
    
    if( $line =~ /\\\\/ ){
        $returnval = 3;
    }
       
    return $returnval;
}


1; # ChkInf.pm loaded with no errors

__END__
