#!/usr/bin/perl -w

@projs = `find ./ -name \"*.sln\"`;

#Modules
for ( $i = 0; $i < @projs; ++$i ){
	if ( $projs[ $i ] =~ /Modules/ ){
		open OUT, ">buildTmp.bat";
		chmod 0777, "buildTmp.bat";
		print OUT "call \"%VS80COMNTOOLS%\\vsvars32.bat\"\r\n";
		print OUT "devenv /Rebuild Debug $projs[ $i ]\r\n";
		print OUT "devenv /Rebuild Release $projs[ $i ]\r\n";
		close OUT;
		system("./buildTmp.bat");
		$projs[ $i ] = ""; #èIÇÌÇ¡ÇΩÇ©ÇÁè¡Ç∑
	}
}

#others
for ( $i = 0; $i < @projs; ++$i ){
	if ( $projs[ $i ] =~ /.sln/ ){
		open OUT, ">buildTmp.bat";
		chmod 0777, "buildTmp.bat";
		print OUT "call \"%VS80COMNTOOLS%\\vsvars32.bat\"\r\n";
		print OUT "devenv /Rebuild Debug $projs[ $i ]\r\n";
		print OUT "if ERRORLEVEL 1 exit /b 1\r\n";
		print OUT "devenv /Rebuild Release $projs[ $i ]\r\n";
		print OUT "if ERRORLEVEL 1 exit /b 1\r\n";
		close OUT;
		system("./buildTmp.bat");
	}
}

