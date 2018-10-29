# (C) Copyright 1999-2003 Microsoft Corporation

package DISPLAY;

use strict;                # Keep the code clean
no strict 'refs';          # no strict references

sub FALSE { return(0); }     # Utility subs
sub TRUE  { return(1); }

#---------------------------------------------------#
# Module level variables
#---------------------------------------------------#
my $Version  = "5.1.2250.0"; # Version of this file
my $DEBUG    = 0;            # Set to 1 for debugging output

my $Current_INF_File;       # Stores name of the current INF file.
my $Current_HTM_File;       # Name of the output file to use.

my @Device_Class_Options;   # Array to store device specific options.

my $Models = {};            # Hash for all models, keyed on models-section-name[.TargetOS]

my %DisplayErrorTable  = (
   6001 => ["Directive %s invalid in this section."],
   6002 => ["%s takes a single numeric parameter."],
   6003 => ["The only values %s allows are 0 and 1."],
   6004 => ["Applet extensions should be installed to HKLM\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Controls Folder\\Display."],
   6005 => [" Section %s does not contain InstalledDisplayDrivers directive."],
   6006 => [" CapablilityOverride should contain value of 0x8 but contains value %s"],
   6007 =>["UseXPModel should not be set"],
   6008 =>["UseVSConverter should not be set"],
   6009 =>["UsePSConverter should not be set"],
   6010 =>["CapabilityOverride is not defined."],
   6011 =>["%s should not be greater than 16"],
   6012 =>["StartType value should be set as 3 (SERVICE_DEMAND_START) for Vista display adapter inf's"],
   6013 =>["Device name used here (%s) should be updated. For Vista inbox display inf's: %s needs to be appended to the Device Name"],
   6014 =>["FeatureScore Directive's value should be F8 for inbox WDDM drivers"],
   6015 =>["Install section should have FeatureScore Directive"],
   6016 =>["Copy Flags value (0x00004000) should be specified for WDDM user mode binaries"],
   6017 =>["Inbox display INF's should not reference LayoutFile "],
   6018 =>["Inbox display INF's should not reference CatalogFile"],
   6019 =>["All inbox Vista inf's should have '3426=windows cd' entry in SourceDisksNames"],
   6020 =>["All inbox Vista inf's should use 3426 entry in SourceDisksFiles"],
   6021 =>["Install section should have RegisterDlls Directive for WDDM drivers "],
   6022 =>["32 bit cloneviewhelper dll should be registered for 32 bit OS and 64 bit for 64 bit OS"],
   6023 =>["CapabilityOverride directive is only applicable for XDDM inf's"],
   6024 =>["FeatureScore Directive's value should be FD for inbox XDDM drivers"],
   6025 =>["FeatureScore Directive's value should be FC for 3rd party XDDM drivers"],
   6026 =>["FeatureScore Directive's value should be F6 for 3rd party WDDM drivers"],
   6027 =>["Inbox Vista inf's date for DriverVer in Version section should not be greater than 08\\21\\2006"],
   6028 =>["Running on Pre Vista OS, should not have Feature Score directive"],
   6029 =>["Running on Vista or later, should have Feature Score directive"],
   6030 =>["FeatureScore Directive's value is unrecognized"],
   6031 =>["FeatureScore information does not match inf type - Expected WDDM FS, got XDDM"],
   6032 =>["FeatureScore information does not match inf type - Expected XDDM FS, got WDDM"],
   );

my %FeatureScoreTable = (
   'D6' => {'type' => 'WDDM', 'ver'  => 2,      'from' => 'WHQL' },
   'DC' => {'type' => 'WDDM', 'ver'  => 2,      'from' => 'Inbox' },
   'E6' => {'type' => 'WDDM', 'ver'  => 1.1,    'from' => 'WHQL' },
   'EC' => {'type' => 'WDDM', 'ver'  => 1.1,    'from' => 'Inbox' },
   'F4' => {'type' => 'WDDM', 'ver'  => 1,      'from' => 'Inbox' },
   'F6' => {'type' => 'WDDM', 'ver'  => 1,      'from' => 'WHQL' },
   'F8' => {'type' => 'WDDM', 'ver'  => 1,      'from' => 'Inbox' },
   'FC' => {'type' => 'XDDM', 'ver'  => undef,  'from' =>  'third party' },
   'FD' => {'type' => 'XDDM', 'ver'  => undef,  'from' => 'Inbox' },
   'FE' => {'type' => 'VGA',  'ver'  => undef,  'from' => undef },
   'FF' => {'type' => 'Default or No Score' , 'ver'  => undef, 'from' => undef }
   );

my %DisplayWarningTable =(
   6101 => ["Client SKU (0x00000001) required for MS Inbox drivers"]
   );

#Variable indicating the OS version
my %OSVer = ('Maj'=> undef, 'Min' => undef);

#array conatining user mode dll names
my @UserModeDlls;

# Holds a list of all unique RegisterDll Sections - hash because it ensures section names are unique
my %RegisterDllSections = ();

# Determine if this is a WDDM or XDDM inf
my $IsWDDMInf;

#---------------------------------------------------#
# Routine for checking device class specific
# directives that occur in common sections verified
# by ChkINF.  Return 0 if directive is unknown or
# 1 if the directive is known (you should also verify
# it and write any appropriate warnings/errors.
#---------------------------------------------------#
sub DCDirective
{
   return(0); # return 1 on success
}

#-------------------------------------------------------------------------------------------------#
# Main Entry Point to Device Specific Module. REQUIRED!                                           #
#-- InfCheck() -----------------------------------------------------------------------------------#
sub InfCheck
{
   my($Item);
   my $Section;
   my $SkipUndecorated = 0;


   print(STDERR "\tInvoking Display::InfCheck...\n") if ($DEBUG);
   
   # Reset variables used to track file state
   $IsWDDMInf = 0;
   %RegisterDllSections = ();
   @UserModeDlls = ();
   %OSVer = ('Maj'=> undef, 'Min' => undef);
   $Models = {};            # Hash for all models, keyed on models-section-name[.TargetOS]

   # Parse the OS version and store in %OSVer
   GetOSVersion();

   # Populate a list of all models found in the INF
   LearnModels();

   if ( $Current_INF_File->sectionDefined("GeneralConfigData") )
   {
      # We should work with this && .NTALPHA if exists
      VerifyGCData("GeneralConfigData");
   }

   
   ChkUserModeDllFlag(); # Checks copy flag and sets the $IsWDDMInf flag

   # Check setting based on model-specific information
   CheckAllModels();

   ChkUserModeDllFlag(); # Verify that the copy flag is set correctly now that we know if feature score is set
   ChkInstalledDisplayDriver(); #check to see if InstalledDisplayDriver exists (both XDDM/WDDM)
   ChkRegKeys(); #verifies debug related registry keys are not set (both XDDM/WDDM)
   ChkCapabilityOverride(); #check to see if CapabilityOverride exists (only XDDM)
   
   # The clone view helper only applies to vista
   if ($OSVer{'Maj'} == 6 && $OSVer{'Min'} == 0) # Vista is 6.0, Win7 is 6.1
   {
      ChkTMMDlls(); 
   }


   #Tests for inbox display INF's
   if(ChkInf::GetGlobalValue("MS_INTERNAL"))
   {
      #ChkDeviceNamingScheme(); # Checks the device naming scheme on Vista (both XDDM/LDDM)
      ChkVersionSection();
      ChkSourceDisksNames();
      ChkSourceDisksFiles();
   }
}

#---------------------------------------------------#
#Verifies the driver does not set following registry keys:
#HKLM\CurrentControlSet\Control\GraphicsDrivers\UseXPModel
#HKLM\Software\Microsoft\Direct3D\UseVSConverter
#HKLM\Software\Microsoft\Direct3D\UsePSConverter
#---------------------------------------------------#

sub ChkRegKeys
{
   my %sections = $Current_INF_File->getSections();
   my $section;
   my $count=0;
   my @lines;
   my $line;
   my($RegRoot, $SubKey, $Name, $Flags, $Value);

   print(STDERR "\tInvoking Display::ChkRegKeys...\n") if ($DEBUG);

   foreach $section (%sections)
   {
      $count = 0;

      next if !$section; # Skip blank sections
      @lines = $Current_INF_File->getSection($section);
      foreach $line (@lines)
      {
         $count++ , next if ($line =~ /^\s*$/); # Skip if it's a blank line

         ($RegRoot, $SubKey, $Name, $Flags, $Value) = ChkInf::GetList($line,$Current_INF_File->getIndex($section ,$count));

         if ((uc $RegRoot eq "HKLM")&& ($SubKey =~ /SYSTEM\\CurrentControlSet\\Control\\GraphicsDrivers/i) && ($Name eq "UseXPModel"))
         {
            if($Value == 1)
            {
               AddDisplayError(6007, $Current_INF_File->getIndex($section ,$count),$section );
            }
         }

         if ((uc $RegRoot eq "HKLM")&& ($SubKey =~ /Software\\Microsoft\\Direct3D/i))
         {
            if(($Name eq "UseVSConverter")&&($Value == 1))
            {
               AddDisplayError(6008, $Current_INF_File->getIndex($section ,$count),$section );
            }
            if(($Name eq "UsePSConverter")&&($Value == 1))
            {
               AddDisplayError(6009, $Current_INF_File->getIndex($section,$count),$section );
            }
         }
         $count++;
      }
   }
}

#---------------------------------------------------#
# Verifies that the module contains a "CapabilityOverride"
# directive in each SoftwareDeviceSettings section.
#---------------------------------------------------#
sub ChkCapabilityOverride
{

   my %sections = $Current_INF_File->getSections();
   my $LastValidSection;
   my $section;
   my $count = 0;
   my $find = 0;
   my @lines;
   my $line;
   my ($RegRoot, $SubKey, $Name, $Flags, $Value);
   
   print(STDERR "\tInvoking Display::ChkCapabilityOverride...\n") if ($DEBUG);

   foreach $section (%sections)
   {
      next if($section !~ /SoftwareDeviceSettings/i);
      $LastValidSection = $section;
      $count = 0;
      $find = 0;
      @lines = $Current_INF_File->getSection($section);
      foreach $line (@lines)
      {
         $count++ , next if ($line =~ /^\s*$/); # Skip if it's a blank line

         if($line =~ /CapabilityOverride/i)
         {
            $find++;
            if($IsWDDMInf)
            {
               #This directive is only applicable in XDDM
               AddDisplayError(6023, $Current_INF_File->getIndex($section,$count),$Value);
            }
            elsif(ChkInf::GetGlobalValue("MS_INTERNAL"))
            {
               ($RegRoot, $SubKey, $Name, $Flags, $Value) = ChkInf::GetList($line,$Current_INF_File->getIndex($section ,$count));
               if(($Value ne "0x8") && ($Value != 8))
               {
                  AddDisplayError(6006, $Current_INF_File->getIndex($section,$count),$Value);
               }
            }
         }
         $count++;
      }

      if(($find == 0) && (!$IsWDDMInf) && (ChkInf::GetGlobalValue("MS_INTERNAL")))
      {
         AddDisplayError(6010, $Current_INF_File->getIndex($LastValidSection,$count-2));
      }
   }
}

#---------------------------------------------------#
# Verifies that the module contains a "InstalledDisplayDrivers"
# directive in each SoftwareDeviceSettings section.
#---------------------------------------------------#
sub ChkInstalledDisplayDriver
{

   my $section = 'SoftwareDeviceSettings';

   print(STDERR "\tInvoking Display::ChkInstalledDisplayDriver...\n") if ($DEBUG);

   return unless $Current_INF_File->sectionDefined($section);
   foreach my $line ($Current_INF_File->getSection($section))
   {
      if($line =~ /InstalledDisplayDrivers/i)
      {
         return; # Section exists, things are okay
      }
   }

   AddDisplayError(6005, $Current_INF_File->getIndex($section,0),$section);
}

#-------------------------------------------------------------------------------------------------------#
# ChkStartType verifies that StartType is set to 3 (in ServiceInstall section) for LH display adapter inf's
# They do not print any error if the AddService/StartType directive is not defined since chkinf.pm module already does that.
#-------------------------------------------------------------------------------------------------------#

sub ChkStartType
{
   my $section = "$_[0].Services";
   my $count = 0;
   
   print(STDERR "\tInvoking Display::ChkStartType...\n") if ($DEBUG);

   return unless $Current_INF_File->sectionDefined($section);
   foreach my $line ($Current_INF_File->getSection($section))
   {
      $count++ , next if ($line =~ /^\s*$/); # Skip if it's a blank line
      if($line =~ /AddService/i)
      {
         # AddService=ServiceName,[flags],service-install-section[,event-log-install-section[,[EventLogType][,EventName]]]
         my($ServiceName, $Flags, $SISec, $ELSec, $ELType, $EName) =
            ChkInf::GetList($line,$Current_INF_File->getIndex($section,$count));

         if (defined($Flags))
         {
            # Handle the blank case
            if ($Flags =~ /^\s*$/)
            {
               $Flags=0;
            }

            if ($Flags=~/^\d*$/)
            {
               # Force numeric strings to numeric context
               #(When a scalar contains just a number, there exists abiguity:
               #is it a string of characters (that happen to be numbers) or is it a number);
               #The addition operator forces its operands into numeric context

               $Flags+=0;
            }

            if ($Flags =~ /^0x/i)
            {
               $Flags =~ s/^0x//i;
               #Covert the string to hex
               $Flags = hex($Flags);
            } 
            elsif ($Flags =~ /^0/)
            {
               $Flags =~ s/^0//i;
               #Convert string to Octal number
               $Flags = oct($Flags);
            } # Otherwise, assume it's already in decimal

            # 
            if (!(ChkInf::is_numeric($Flags)) || $Flags =~/^\s*$/ )
            {
               $Flags=0;
            }

            if(defined($SISec) && ($SISec !~ /^$/))
            {
               if($Current_INF_File->sectionDefined($SISec))
               {
                  #Flags specifies one or more (ORed) system-defined flags
                  if (($Flags & 0x2)== 0x2)
                  {
                     #PNP drivers have flag value set to 0x00000002 and  StartType check only needs to be done for PnP
                     my $SIcount = 0;
                     foreach my $ServiceLine ($Current_INF_File->getSection($SISec))
                     {
                        $SIcount++ , next if ($ServiceLine =~ /^\s*$/); # Skip if it's a blank line
                        if($ServiceLine =~ /StartType/i)
                        {
                           my ($Directive, $Value) = ChkInf::SplitLine($SISec,$count,$ServiceLine);
                           if($Value != 3)
                           {
                              AddDisplayError(6012, $Current_INF_File->getIndex($SISec,$SIcount),$Value);
                           }
                        }
                        $SIcount++;

                     }
                  }
               }
            }
         }
      }
      $count++;
   } 
}


#---------------------------------------------------------------------#
# Verifies that the WDDM inf contains new copy flag to Support PNPStop Directive
# This flag (0x00004000) is only required for user mode binaries
#---------------------------------------------------------------------#
sub ChkUserModeDllFlag
{
   #Check the DestinationDirs section to get the file-list-section:
   #[DestinationDirs]
   #[DefaultDestDir=dirid[,subdir]]
   #[file-list-section=dirid[,subdir]]

   my $section = 'DestinationDirs';
   my @Values;

   print(STDERR "\tInvoking Display::ChkUserModeDllFlag...\n") if ($DEBUG);

   return unless ($Current_INF_File->sectionDefined($section));

   my $DestDirsCount = 0;
   foreach my $DestDirsLine ($Current_INF_File->getSection($section))
   {
      $DestDirsCount++ , next if ($DestDirsLine =~ /^\s*$/); # Skip if it's a blank line
      my ($FileListSec, $dirid) = ChkInf::SplitLine($section,$DestDirsCount,$DestDirsLine);

      #$Directive contains the file-list-section and $Value contains dirid,[,subdir]]
      @Values = ChkInf::GetList($dirid,$Current_INF_File->getIndex($section, $DestDirsCount));

      #Get the dirid
      my $dirid  = shift(@Values);

      #Check the file section only for user mode copy file section (dirid == 11 for system32 or dirid==10 for syswow)
      if ( $Current_INF_File->sectionDefined($FileListSec) && (($dirid == 11) || ($dirid == 10)))
      {
         my $FileListCount = 0;
         #Check the file-list-section for CopyFlag
         foreach my $FileListLine ($Current_INF_File->getSection($FileListSec))
         {
            $FileListCount++ , next if ($FileListLine =~ /^\s*$/); # Skip if it's a blank line
            if($FileListLine =~ /.dll/i)
            {
               # 11 is equivalent to %windir%\system32 for NT-based systems, and to %windir%\system for Windows 9x/Me;
               # We only want to add the dll's in System32 folder
               if($dirid == 11)
               {
                  # Add the user mode dll names to our global array
                  my @Values = split(/,/,$FileListLine);
                  my $DllName = shift(@Values);
                  push(@UserModeDlls,$DllName);
               }

               if($FileListLine =~ /4000/  && $OSVer{'Maj'} > 5 )
               {
                  #The inf contains new Copy File flag for user mode WDDM binary. It is WDDM inf
                  $IsWDDMInf = 1;
               }
               elsif($IsWDDMInf && !($FileListLine =~ /4000/)) # Only executes on second run of this method
               {
                  # generate an error since line doesn't have the copy flag
                  AddDisplayError(6016, $Current_INF_File->getIndex($FileListSec,$FileListCount),$FileListLine);
               }
               else
               {
                  # Left empty - it's an XDDM driver
               }
            }
            $FileListCount++;
         }
      }
   }
}

#-----------------------------------------------------------------------------#
# This method checks a given feature score against currently known settings
#-----------------------------------------------------------------------------#
sub ChkFeatureScore
{
   my ($Value,$section,$count) = @_;

   print(STDERR "\tInvoking Display::ChkFeatureScore ($Value)...\n") if ($DEBUG);

   # Create a reference to the anonymous hash stored at $Value in %FeatureScoreTable
   my $score = \%{ $FeatureScoreTable{ $Value } };

   if (!defined(%{$score})) # %{$foo} dereferences, allowing to check for perl's "null" (undef)
   {
      # FeatureScore is an unrecognized value
      AddDisplayError(6030, $Current_INF_File->getIndex($section,$count),$Value);
   }
   elsif ($IsWDDMInf) # Windows Display Driver Model (formerly LDDM for Longhorn DDM)
   {
      if ($score->{'type'} ne "WDDM")
      {
         #FeatureScore information does not match inf type
         AddDisplayError(6031, $Current_INF_File->getIndex($section,$count),$Value);
      }
      elsif (ChkInf::GetGlobalValue("MS_INTERNAL"))
      {
         if ($score->{'from'} ne "Inbox")
         {
            #Internal drivers must come from the inbox
            AddDisplayError(6014, $Current_INF_File->getIndex($section,$count),$Value);
         }
      }
      elsif ($score->{'from'} ne "WHQL" && $score->{'from'} ne "Inbox")
      {
         #Non internal drivers must be verified by WHQL
         AddDisplayError(6026, $Current_INF_File->getIndex($section,$count),$Value);
      }
   }
   else #should be of type XDDM/third party
   {
      if ($score->{'type'} ne "XDDM") # XP Device Driver Model
      {
         #FeatureScore information does not match inf type
         AddDisplayError(6032, $Current_INF_File->getIndex($section,$count),$Value);
      }
      else
      {
         if (ChkInf::GetGlobalValue("MS_INTERNAL"))
         {
            if ($score->{'from'} ne "Inbox")
            {
               AddDisplayError(6024, $Current_INF_File->getIndex($section,$count),$Value);
            }
         }
         elsif ($score->{'from'} ne "third party")
         {
            # Non-internal drivers must be from a third party
            AddDisplayError(6025, $Current_INF_File->getIndex($section,$count),$Value);
         }
      }
   }
}

sub ChkNameScheme
{
   my $count   =     0;
   my $section = 'Manufacturer';
   
   foreach my $line ($Current_INF_File->getSection($section))
   {
      $count++, next if ($line =~ /^[\s\t]*$/);# Skip if it's a blank line

      my ($Directive,@ErrorArgs);

      if( (!$IsWDDMInf) && (($Directive !~ /Microsoft\s*Corporation/i) || ($Directive !~ /XDDM/)))
      {
         $ErrorArgs[1] = "'XDDM (Microsoft Corporation)'";
         AddDisplayError(6013, $Current_INF_File->getIndex($section,$count), @ErrorArgs);
      }

      if( ($IsWDDMInf) && (($Directive !~ /Microsoft\s*Corporation/i) || ($Directive !~ /WDDM/)))
      {
         $ErrorArgs[1] = "'(Microsoft Corporation - WDDM)'";
         AddDisplayError(6013, $Current_INF_File->getIndex($section,$count), @ErrorArgs);
      }

      $count++;
   }
}

#--------------------------------------------------------------------#
# Verifies that inbox display inf's do not reference LayoutFile or CatalogFile
#--------------------------------------------------------------------#
sub ChkVersionSection()
{
   my $section = 'Version';

   my $count = 0;
   my @lines;
   my $line;
   my $value;

   @lines = $Current_INF_File->getSection($section);
   foreach $line (@lines)
   {
      $count++ , next if ($line =~ /^\s*$/); # Skip if it's a blank line
      if($line =~ /LayoutFile/i)
      {
         AddDisplayError(6017, $Current_INF_File->getIndex($section,$count-1),$section);
      }
      elsif($line =~ /CatalogFile/i)
      {
         AddDisplayError(6018, $Current_INF_File->getIndex($section,$count-1),$section);
      }
      elsif($line =~ /DriverVer/i)
      {
         # This currently only supports Vista
         # TODO: Add support for Win7 when the release date is known
         next if ($OSVer{'Maj'} < 6 || ($OSVer{'Maj'} >= 6 && $OSVer{'Min'} >= 1));

         my ($Directive, $Value) = ChkInf::SplitLine($section,$count,$line);
         #$Value is of form (Month)/(Day)/(Year,Version)
         #Main ckinf module checks for most invalid date scenarios (i.e. if date is two digit and is > 31, month > 12 and so on
         #So simply check for 08\21\2006

         my(@Values)  = split(/\//, $Value);

         #$Values[2] is of form (Year,Version)
         my(@Values1) = split(/,/, $Values[2]);

         # Make sure the date is not greater than 08\21\2006
         if ($Values1[0]  > 2006)
         {
            AddDisplayError(6027, $Current_INF_File->getIndex($section,$count),$line);
         } 
         elsif ($Values1[0]  == 2006)
         {
            if ($Values[0] > 8)
            {
               AddDisplayError(6027, $Current_INF_File->getIndex($section,$count),$line);
            }
            elsif ($Values[0] == 8)
            {
               if ($Values[1] > 21)
               {
                  AddDisplayError(6027, $Current_INF_File->getIndex($section,$count),$line);
               }
            }
         }
      }
      $count++;
   }
}

#---------------------------------------------------------------------------------#
# Verifies that Vista inbox display inf's contain '3426=windows cd' entry in SourceDiskNamesSection
#---------------------------------------------------------------------------------#
sub ChkSourceDisksNames()
{
   my $section = 'SourceDisksNames';

   print (STDERR "Invoking Display::ChkSourceDisksNames...") if ($DEBUG);

   my $count = 0;
   my @lines;
   my $line;
   my $value;
   my ($Directive, $Value);

   return unless $Current_INF_File->sectionDefined($section);
   foreach my $line ($Current_INF_File->getSection($section))
   {
      $count++ , next if ($line =~ /^\s*$/); # Skip if it's a blank line
      ($Directive, $Value) = ChkInf::SplitLine($section,$count,$line);

      if(($Directive != 3426) || ($Value !~ /windows[\s|\t]*cd/i))
      {
         AddDisplayError(6019, $Current_INF_File->getIndex($section,$count-1),$section);
      }
      $count++;
   }
}

sub ChkSourceDisksFiles()
{
   my $section = 'SourceDisksFiles';
   my $count = 0;
   my @lines;
   my $line;
   my $value;
   my ($Directive, $Value);

   return unless $Current_INF_File->sectionDefined($section);
   foreach my $line ($Current_INF_File->getSection($section))
   {
      $count++ , next if ($line =~ /^\s*$/); # Skip if it's a blank line
      ($Directive, $Value) = ChkInf::SplitLine($section,$count,$line);

      if($Value != 3426)
      {
         AddDisplayError(6020, $Current_INF_File->getIndex($section,$count-1),$section);
      }
      $count++;
   }

}

#---------------------------------------------------#
# Verifies that native CloneViewHelper dll is registered
#---------------------------------------------------#
sub ChkTMMDlls
{
   # This sub uses a global array filled by ChkUserModeDllFlag sub
   if(scalar(@UserModeDlls) == 0)
   {
      return;
   }


   foreach my $RegisterDllSection (keys %RegisterDllSections)
   {
      my $count = 0;
      my $FinalCount =0;
      my $found = 0;

      foreach my $line ($Current_INF_File->getSection($RegisterDllSection))
      {
         $count++, next if ($line =~ /^[\s\t]*$/);# Skip if it's a blank line

         #Skip the blank lines at end of section and display error at right place
         $FinalCount = $count;
         
         my @Values     = ChkInf::GetList($line,$Current_INF_File->getIndex($RegisterDllSection,$count));
         foreach my $Value (@Values)
         {
            if($Value =~ /.dll/i)
            {
               #Check if this dll is in list of UseMode dlls copied to system32 folder
               my $i;
               for($i = 0; $i < scalar(@UserModeDlls); $i++)
               {
                  if($UserModeDlls[$i] eq $Value)
                  {
                     $found++;
                  }
               }
            }
        }

         $count++;
      }

      if($found < 1)
      {
         AddDisplayError(6022, $Current_INF_File->getIndex($RegisterDllSection,$FinalCount),$RegisterDllSection);
      }
   }

}



#---------------------------------------------------#
# Verifies that the module loaded correctly.  Be sure
# to change the string to the name of your module.
#---------------------------------------------------#
sub Exists
{
   print(STDERR "Module \"DISPLAY\" Loaded\n");
   return(1); # return 1 on success
}


#---------------------------------------------------#
# Is called when module is first included.
# Use it to set your per-inf variables.
#---------------------------------------------------#
sub InitGlobals
{
   # First parameter is the INF object
   $Current_INF_File=$_[1];

   # Output file = htm subdir + ((INF Name - .inf) + .htm)
   $Current_HTM_File = $Current_INF_File->{inffile};
   $Current_HTM_File = substr($Current_HTM_File, rindex($Current_HTM_File,"\\")+1);
   $Current_HTM_File =~ s/\.inf$/\.htm/i;
   $Current_HTM_File = "htm\\" . $Current_HTM_File;

   # Second parameter is a list of Device Specific options.
   my($DC_Specific_Options) = $_[2];

   # $DC_Specific_Options is a string that contains all device
   #   class options delimited by &'s
   if (defined($DC_Specific_Options))
   {
      @Device_Class_Options = split(/&/,$DC_Specific_Options);
   } 
   else
   {
      @Device_Class_Options = ("NULL", "NULL");
   }

   # Array must be shifted since first element also had a & prepended
   shift(@Device_Class_Options);

   return(1); # return 1 on success
}


#---------------------------------------------------#
# Allows to add Device specific information to the
# top of the INF summary page.  The information here
# should be brief and link to detailed summaries
# below. (Which will be written using PrintDetials).
#---------------------------------------------------#
sub PrintHeader
{
   return(1); # return 1 on success
}


#---------------------------------------------------#
# Allows addition of device specific results to the
# detail section on the INF summary.
#---------------------------------------------------#
sub PrintDetails
{
   return(1); # return 1 on success
}

###################################################################################################
#+-----------------------------------------------------------------------------------------------+#
#|                                    WORK SUBROUTINES                                           |#
#+-----------------------------------------------------------------------------------------------+#
###################################################################################################

sub CheckAllModels
{
   print(STDERR "\tInvoking Display::CheckAllModels...\n") if($DEBUG);
  

   my $VistaSeen = 0;
   # Have to check all target OSes prior to using them. This way, we can differentiate
   # between an inf that expects to use the default (untargetted/decorated) and 
   # one that uses target differentiation for XP/Vista/Win7
   # This prevents featurescore errors triggering on pre-vista INF files
   foreach my $TargetOS (keys(%$Models))
   { 
      my ($MdlSecName,$NTARCH,$OSMaj,$OSMin,$OSType,$Suite) = split(/\./,$TargetOS);
      
      $OSMaj += 0; # Convert to scalar and/or set a value

      if ($OSMaj != 0 && $OSMaj >= 6)
      {
         $VistaSeen = 1;
      }
   }

   # $Models is a hash reference where $Models->{TargetOS}->{HardwareID} is a unique descriptor
   # for a particular device. Loop through all descriptors, grouped by TargetOS.
   foreach my $TargetOS (keys(%$Models))
   {
      # $ModelList holds the hash reference to all devices, keyed by hardware IDs.
      my $ModelList = $Models->{$TargetOS};
      my ($MdlSecName,$NTARCH,$OSMaj,$OSMin,$OSType,$Suite) = split(/\./,$TargetOS);
      $OSMaj += 0;

      foreach my $HardwareID ( keys(%$ModelList) )
      {
         # $Device holds the hash reference to the specific device's hash
         my $Device = $ModelList->{$HardwareID};

         # SetupAPI automatically adds relevant architecture platform to install section
         # Therefore, emulate this behavior by trying for all possible arch targets 
         foreach my $TargetArch ("",".nt",".ntx86",".ntia64",".ntamd64")
         {
            # Add ".TargetArch" to install section if target architecture isn't empty
            my $InstSecName = $Device->{'DDINSTALL'} . $TargetArch;

            # Skip this architecture target if we can't find the install section name
            next unless $Current_INF_File->sectionDefined($InstSecName);

            # This section specifically targets Vista+, FS required regardless of OS the INF is running on
            # OR: This OS is Vista, and no specific target for vista seen, require FS
            if ($OSMaj >= 6 || ($OSVer{'Maj'} >=6 && !$VistaSeen))
            {
               # CheckSingleModel(Install Section Name, Allow FS, Require FS)
               CheckSingleModel($InstSecName,1,1);
            }
            # This OS is Vista+ and a specific Vista+ section has been seen
            # Feature Score is allowed but not required, as the fallback section may or may not be for XP
            elsif ($OSVer{'Maj'} >= 6 && $VistaSeen)
            {
               CheckSingleModel($InstSecName,1,0);
            }
            # This section is not allowed to have feature score
            else
            {
               CheckSingleModel($InstSecName,0,0);
            }
            
            # Vista+ start type is required to be SERVICE_DEMAND_START (3)
            if ($IsWDDMInf)
            {
               ChkStartType($InstSecName);
            }
         } # END: foreach my $TargetArch ...

      }  # END: foreach my $HardwareID ( keys(%{$Models->{$TargetOS}}) )
   } # END: foreach my $TargetOS (keys(%$Models))
}

sub CheckSingleModel
{
   my $InstSecName = shift;
   my $AllowFS = shift;
   my $RequireFS = shift;

   print(STDERR "\tInvoking Display::CheckSingleModel ($InstSecName)...\n") if($DEBUG);

   # Check for device-specific software settings and configuration data
   if ( $Current_INF_File->sectionDefined($InstSecName.".SOFTWARESETTINGS") )
   {
      VerifySoftwareSettings($InstSecName.".SOFTWARESETTINGS");
   }

   if ( $Current_INF_File->sectionDefined($InstSecName.".OPENGLSOFTWARESETTINGS") )
   {
      VerifySoftwareSettings($InstSecName.".OPENGLSOFTWARESETTINGS");
   }

   if ( $Current_INF_File->sectionDefined($InstSecName.".GENERALCONFIGDATA") )
   {
      VerifyGCData($InstSecName.".GENERALCONFIGDATA");
   }

   my $count = 0;
   my $FinalCount = 0;

   my @RegLines;
   my $FSFound = 0; # Has the feature score been found?
   my $RegDllFound = 0; # Have any dlls been found?

   foreach my $line ($Current_INF_File->getSection($InstSecName))
   {
      $count++, next if ($line =~ /^\s*$/); # Skip if it's a blank line
      
      $FinalCount = $count; # Store last line of section for error output

      my ($Directive,$Value) = ChkInf::SplitLine($InstSecName,$count,$line);

      if (uc $Directive eq "ADDREG")
      {
         foreach ( ChkInf::GetList($Value,$Current_INF_File->getIndex($InstSecName,$count)) )
         {
            push(@RegLines, RegLines($_)) if ( $Current_INF_File->sectionDefined($_) );
         }
      }

      if (uc $Directive eq 'FEATURESCORE')
      {
         $FSFound = 1;
         ChkFeatureScore($Value,$InstSecName,$count);
      }

      if (uc $Directive eq 'REGISTERDLLS')
      {
         # Using a hash guarantees uniqueness of sections
         $RegisterDllSections{$Value} = 1;
         $RegDllFound = 1;
         
      }

      $count++;
   }
   

   if ($AllowFS && $RequireFS && !$FSFound)
   {
      # Running on Vista OS, FS required
      AddDisplayError(6029, $Current_INF_File->getIndex($InstSecName,$FinalCount),'FeatureScore');
   }
   elsif (!$AllowFS && $FSFound)
   {
      #print "Running on Pre Vista OS; should not have Feature Score directive";
      AddDisplayError(6028,$Current_INF_File->getIndex($InstSecName,$FinalCount),'FeatureScore');
   }

   if ($IsWDDMInf && !$RegDllFound)
   {
      # WDDM drivers are required to have a registerdlls directive
      AddDisplayError(6021, $Current_INF_File->getIndex($InstSecName,$FinalCount),'RegisterDlls');  
   }


   # Check all AddReg lines to make sure none touch the following location
   foreach (@RegLines)
   {
      if ($$_[0] =~ /^HKLM\s*,\s*"?SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Controls\sFolder\\Device/i)
      {
         AddDisplayError(6004,$$_[1]);
      }
   }
}

#-------------------------------------------------------------------------------------------------#
# GetOSVersion - Fills the global %OSVer with major, minor, and build information                 #
#-- GetOSVersion() -------------------------------------------------------------------------------#
sub GetOSVersion
{
   my @tokens;

   # Get the OS version
   # Pipe the output of the 'ver' command to OSVER
   open (OSVER, "ver |");

   # The first line contains garbage
   my $t = <OSVER>;

   # This is the actual output text from 'ver'. Chop the newline character.
   my $ver = <OSVER>;
   chomp($ver);
   close (OSVER);

   # Retrieve the version from the text
   @tokens = split(/ /, $ver);

   my $ver1 = pop(@tokens);

   my ($MajVer,$MinVer,$Build) = split(/\./,$ver1);

   $OSVer{'Maj'} = $MajVer;
   $OSVer{'Min'} = $MinVer;
   $OSVer{'Build'} = $Build;
}


#-------------------------------------------------------------------------------------------------#
# LearnModels - Returns an array of all models listed in the INF                                  #
#-- LearnModels() --------------------------------------------------------------------------------#
sub LearnModels
{
   my $section = 'Manufacturer';
   my $count   = 0;

   my($Directive, $Value, @Targets);

   print(STDERR "\tInvoking Display::LearnModels...\n") if ($DEBUG);

   foreach my $line ($Current_INF_File->getSection($section))
   {
      $count++, next if ($line =~ /^\s*$/); # Skip if it's a blank line

      ($Directive,$Value) = ChkInf::SplitLine($section,$count,$line);

      # Whistler allows new TargetDecorations.
      @Targets = ChkInf::GetList($Value,$Current_INF_File->getIndex($section, $count));
      # Value holds the undecorated section, @Values holds TargetDecorations.
      $Value  = shift(@Targets);

      # [Manufacturers] may not have both a directive & a value
      if ($Value =~ /^\s*$/)
      {
         $Value     = $Directive;
         $Directive = "";
      }
      else
      {
         $Value =~ s/\x93/\"/g; # Replace "smart quotes" with straight quotes
         $Value =~ s/\x94/\"/g;
         $Value    =~  s/^\"(.*)\"/$1/;   # And Strip the Quotes!
      }

      if ( $Current_INF_File->sectionDefined($Value) )
      {
         $Models->{$Value} = ExpandModelSection($Value);
      }
      foreach (@Targets)
      {
         if ( $Current_INF_File->sectionDefined("$Value.$_") )
         {
            $Models->{"$Value.$_"} = ExpandModelSection("$Value.$_");
         }

         # Internal devices are required to have a "client" OS type set
         if(ChkInf::GetGlobalValue("MS_INTERNAL"))
         {
            my ($NTARCH,$OSMaj,$OSMin,$OSType,$Suite) = split(/\./,$_);
            
            # Must be set to 1 for all win7 client devices
            if ($OSType != 1)
            {
               AddDisplayWarning(6101,$Current_INF_File->getIndex($section,$count),$Directive);
            }

         }

      }

      $count++;
   }
}

#-------------------------------------------------------------------------------------------------#
#-- VerifyGCData($line, $Model, $Extension) ------------------------------------------------------#
sub VerifyGCData
{
   my $Section     = $_[0];
   my @lines       = $Current_INF_File->getSection($Section);
   my $count       = 0;
   my $line;

   my($Directive, @Values, $Value);

   my(%SecToCheck) = ("MAXIMUMDEVICEMEMORYCONFIGURATION", TRUE,
         "MAXIMUMNUMBEROFDEVICES"          , TRUE,
         "KEEPEXISTINGDRIVERENABLED"       , TRUE,
         "COPYFILES"						 , TRUE,
         "SESSIONIMAGESIZE"				 , TRUE);

   my($CurrentLine, $Temp, @LineItems, $SizeOfArray);
   ChkInf::Mark("$Section");

   print(STDERR "\tInvoking Display::VerifyGCData (@_)...\n") if ($DEBUG);
   foreach $line (@lines)
   {
      $count++, next if ($line =~ /^\s*$/);# Skip if it's a blank line

      ($Directive,$Value) = ChkInf::SplitLine($Section,$count,$line);
      @Values             = ChkInf::GetList($Value,$Current_INF_File->getIndex($Section,$count));

      if (uc($Directive) eq "MAXIMUMDEVICEMEMORYCONFIGURATION")
      {
         if (defined($Values[0]))
         {
            if (defined($Values[1]))
            {
               AddDisplayError(6002,$Current_INF_File->getIndex($Section,$count),$Directive);
            }
         }
         else
         {
            AddDisplayError(6002,$Current_INF_File->getIndex($Section,$count),$Directive);
         }
      }
      elsif (uc($Directive) eq "MAXIMUMNUMBEROFDEVICES")
      {
         if (defined($Values[0]))
         {
            if (defined($Values[1]))
            {
               AddDisplayError(6002,$Current_INF_File->getIndex($Section,$count),$Directive);
            }
         }
         else
         {
            AddDisplayError(6002,$Current_INF_File->getIndex($Section,$count),$Directive);
         }
      }
      elsif (uc($Directive) eq "KEEPEXISTINGDRIVERENABLED")
      {
         if (defined($Values[0]))
         {
            if (defined($Values[1]))
            {
               AddDisplayError(6002,$Current_INF_File->getIndex($Section,$count),$Directive);
            }
            if (($Values[0] != 1) and ($Values[0] != 0))
            {
               AddDisplayError(6003,$Current_INF_File->getIndex($Section,$count),$Directive);
            }
         }
         else
         {
            AddDisplayError(6002,$Current_INF_File->getIndex($Section,$count),$Directive);
         }
      }
      elsif (uc($Directive) eq "SESSIONIMAGESIZE")
      {
         if (defined($Values[0]))
         {
            if (defined($Values[1]))
            {
               AddDisplayError(6002,$Current_INF_File->getIndex($Section,$count),$Directive);
            }
            if($Values[0] > 16)
            {
               AddDisplayError(6011,$Current_INF_File->getIndex($Section,$count),$Directive);
            }
         }
         else
         {
            AddDisplayError(6002,$Current_INF_File->getIndex($Section,$count),$Directive);
         }
      }
      else
      {
         AddDisplayError(6001,$Current_INF_File->getIndex($Section,$count),$Directive)
            unless (defined $SecToCheck{uc $Directive});
      }
      $count++;

   }

   return(TRUE);
}

#-------------------------------------------------------------------------------------------------#
#-- VerifySoftwareSettings($line, $Model, $Extension) --------------------------------------------#
sub VerifySoftwareSettings
{
   my $Section     = $_[0];
   my @lines      = $Current_INF_File->getSection($Section);
   my $count      = 0;
   my $line;

   my($Directive, @Values, $Value);


   my(%SecToCheck) = ("ADDREG",    TRUE,
                      "COPYFILES", TRUE,
                      "NEEDS",     TRUE);

   my($CurrentLine, $Temp, @LineItems, $SizeOfArray);

   ChkInf::Mark("$Section");
#---------------------------------------------------#
# Is called when module is first included.
# Use it to set your per-inf variables.
#---------------------------------------------------#
sub InitGlobals
{
   # First parameter is the INF object
   $Current_INF_File=$_[1];

   # Output file = htm subdir + ((INF Name - .inf) + .htm)
   $Current_HTM_File = $Current_INF_File->{inffile};
   $Current_HTM_File = substr($Current_HTM_File, rindex($Current_HTM_File,"\\")+1);
   $Current_HTM_File =~ s/\.inf$/\.htm/i;
   $Current_HTM_File = "htm\\" . $Current_HTM_File;

   # Second parameter is a list of Device Specific options.
   my($DC_Specific_Options) = $_[2];

   # $DC_Specific_Options is a string that contains all device
   #   class options delimited by &'s
   if (defined($DC_Specific_Options))
   {
      @Device_Class_Options = split(/&/,$DC_Specific_Options);
   } 
   else
   {
      @Device_Class_Options = ("NULL", "NULL");
   }

   # Array must be shifted since first element also had a & prepended
   shift(@Device_Class_Options);

   return(1); # return 1 on success
}


#---------------------------------------------------#
# Allows to add Device specific information to the
# top of the INF summary page.  The information here
# should be brief and link to detailed summaries
# below. (Which will be written using PrintDetials).
#---------------------------------------------------#
sub PrintHeader
{
   return(1); # return 1 on success
}


#---------------------------------------------------#
# Allows addition of device specific results to the
# detail section on the INF summary.
#---------------------------------------------------#
sub PrintDetails
{
   return(1); # return 1 on success
}

   print(STDERR "\tBeginning VerifyModel (@_)...\n") if ($DEBUG);
   foreach $line (@lines)
   {

      $count++, next if ($line =~ /^\s*$/);# Skip if it's a blank line

      ($Directive,$Value) = ChkInf::SplitLine($Section,$count,$line);
      @Values             = ChkInf::GetList($line,$Current_INF_File->getIndex($Section,$count));

      if (uc($Directive) eq "ADDREG")
      {
         # ADDREG is valid, so call the ChkInf ADDREG checking routine
         ChkInf::ADDREG($Section, $count, $Value);
      } 
      elsif (uc($Directive) eq "DELREG")
      {
         # ADDREG is valid, so call the ChkInf ADDREG checking routine
         ChkInf::DELREG($Section, $count, $Value);
      } 
      else
      {
         AddDisplayError(6001,$Current_INF_File->getIndex($Section,$count),$Directive)
            unless (defined $SecToCheck{uc $Directive});
      }

      $count++;

   }
   return(TRUE);
}

###################################################################################################
#+-----------------------------------------------------------------------------------------------+#
#|                                   HELPER SUBROUTINES                                          |#
#+-----------------------------------------------------------------------------------------------+#
###################################################################################################

#-------------------------------------------------------------------------------------------------#
# This sub adds a new error to the list of errors for the file.                                   #
#-- AddDisplayError($ErrorNum, $LineNum, @ErrorArgs) ---------------------------------------------#
sub AddDisplayError
{
   my($ErrorNum) = $_[0], shift;
   my($LineNum)  = $_[0], shift;
   my(@ErrorArgs)= @_;

   $ErrorArgs[0] = " " if (! defined($ErrorArgs[0]));
   my($str, $this_str, $info_err, $format_err);

   $info_err = $DisplayErrorTable{"$ErrorNum"};

   if ( !defined($info_err) )
   {
      $this_str = "Unknown error $ErrorNum.";
   }
   else
   {
      $format_err = $$info_err[0];
      $this_str = sprintf($format_err, @ErrorArgs);
   }

   ChkInf::AddDeviceSpecificError($LineNum, $ErrorNum, $this_str);
}

#-------------------------------------------------------------------------------------------------#
# This sub adds a new warning to the list of warnings for the file.                               #
#-- AddDisplayWarning($WarnNum, $LineNum, @ErrorArgs) --------------------------------------------#
sub AddDisplayWarning
{
   my($WarnNum)  = $_[0], shift;
   my($LineNum)  = $_[0], shift;
   my(@WarnArgs) = @_;

   $WarnArgs[0] = " " if (! defined($WarnArgs[0]));
   my($str, $this_str, $info_wrn, $format_wrn);

   $info_wrn = $DisplayWarningTable{"$WarnNum"};

   if ( !defined($info_wrn) )
   {
      $this_str = "Unknown warning $WarnNum.";
   }
   else
   {
      $format_wrn = $$info_wrn[0];
      $this_str = sprintf($format_wrn, @WarnArgs);
   }
   ChkInf::AddDeviceSpecificWarning($LineNum, $WarnNum, $this_str);
}

#-------------------------------------------------------------------------------------------------#
#-- ExpandModelSection($Section) -----------------------------------------------------------------#
sub ExpandModelSection
{
   my $Section  = shift;
   my @lines    =  $Current_INF_File->getSection($Section);
   my $count    = 0;

   my $Devices  =  {};

   my($InstallSectionName, $HardwareID);

   print(STDERR "\tInvoking Display::ExpandModelSection...\n") if ($DEBUG);

   foreach my $line (@lines)
   {
      $count++, next if ($line =~ /^\s*$/);# Skip if it's a blank line

      # device-description=install-section-name,hw-id[,compatible-id...]
      my ($DevDesc,$Value) = ChkInf::SplitLine($Section,$count,$line);
      my @Values           = ChkInf::GetList($Value,$Current_INF_File->getIndex($Section,$count));

      $Values[0] =~ s/^\s*//i; # Kill leading spaces
      $Values[0] =~ s/\s*$//i; # Kill trailing spaces

      if (defined $Values[0])
      {
         $InstallSectionName = $Values[0];
      } 
      else
      {
         # TODO: ERROR
         next;
      }

      if (defined $Values[1])
      {
         $HardwareID = uc($Values[1]);
      } 
      else
      {
         # TODO: ERROR
         next;
      }

      if (defined($Values[0]))
      {
         if (defined $Devices->{$HardwareID})
         {
            # TODO: ERROR
         } 
         else
         {
            $Devices->{$HardwareID} = {  'DDINSTALL' => $InstallSectionName,
                                         'DESCRIPTION' => $DevDesc };            

            if ($DevDesc =~ /WDDM/i)
            {
               $IsWDDMInf = 1;
            }
         }
      }
      $count++;
   }

   return $Devices;
}

#-------------------------------------------------------------------------------------------------#
# RegLines - returns all reglines (w/ string tokens removed) from a section.                      #
#-- RegLines() -----------------------------------------------------------------------------------#
sub RegLines
{
   my $line;
   my $Section  =  shift;
   my @lines    =  $Current_INF_File->getSection($Section);
   my $count    =  0;
   my @reglines;

   print(STDERR "\tInvoking Media::RegLines...\n") if ($DEBUG);

   foreach $line (@lines)
   {
      #Skip blank lines even though this should never be true.
      $count++, next if ($line =~ /^[\s\t]*$/);
      # Remove all string tokens
      $line = ChkInf::RemoveStringTokens($Current_INF_File->getIndex($Section,$count),$line);
      # Update our list with [$line, $linenum]
      push(@reglines, [$line,$Current_INF_File->getIndex($Section,$count)]);
      $count++;
   }
   return(@reglines);
}

return(1); # Module must return 1 to let chkinf know it loaded correctly.
