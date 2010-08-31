<xsl:stylesheet
                  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                  version="2.0">
            <xsl:output  method="xml"
                        indent="yes"
                        omit-xml-declaration="yes"/>

	    <xsl:template match="/">
		<html style="font-family: SBL Hebrew">
		<h3><u>Strong's Definition</u></h3>
		<br/>
			<xsl:apply-templates/>
		</html>
	    </xsl:template>

	<xsl:template match="/entry">
			<xsl:apply-templates select="w"/> 
			<xsl:apply-templates select="note[@type='x-typo']"/>
			<xsl:apply-templates select="note[@type='exegesis']"/>
			<xsl:apply-templates select="note[@type='explanation']"/>
			<xsl:apply-templates select="note[@type='translation']"/>
			<xsl:apply-templates select="foreign"/>

		<br/>
		<xsl:apply-templates select="list"/>
	</xsl:template>

	<xsl:template match="w">
		<b>
			<xsl:value-of select="@lemma"/>
		</b>
		<br>
		<xsl:value-of select="@morph"/>
		</br>
		<br/>
	</xsl:template>
	
<!--	<xsl:template match="w[@ID]">
		<xsl:value-of select="@morph"/>		
	</xsl:template>
-->
	
	<xsl:template match="note[@type='exegesis']">
		<xsl:apply-templates/>&#160;
	</xsl:template>
		
	<xsl:template match="note[@type='explanation']">
		<xsl:apply-templates/>
	</xsl:template>
	
	<xsl:template match="note[@type='translation']">
		<xsl:apply-templates/>		
	</xsl:template>
		
	<xsl:template match="foreign">
		[Greek:<xsl:apply-templates/>] 
	</xsl:template>
	
	<xsl:template match="list">
		<ul>
			<xsl:apply-templates select="item"/>
		</ul>
	</xsl:template>
	
	<xsl:template match="item">
		<li>
			<xsl:apply-templates/>
		</li>
	</xsl:template>

<!--            <xsl:template  match="*[name()='see']">
		<br/>see
                  <xsl:element  name="a" >
			<xsl:attribute name="href"><xsl:value-of  select="@language"/>://<xsl:value-of select="@strongs_number"/></xsl:attribute>
                        <xsl:apply-templates/>
                  </xsl:element>
            </xsl:template>
-->
      </xsl:stylesheet> 
