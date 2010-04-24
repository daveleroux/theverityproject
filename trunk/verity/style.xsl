<xsl:stylesheet
                  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                  version="2.0">
            <xsl:output  method="xml"
                        indent="yes"
                        omit-xml-declaration="yes"/>

	    <xsl:template match="/">
		<html>
		<h3><u>Strong's Definition</u></h3>
		<br/>
			<xsl:apply-templates/>
		</html>
	    </xsl:template>

	<xsl:template match="/entry">
		<xsl:apply-templates/>
	</xsl:template>

            <xsl:template  match="*">
                  <xsl:copy>
                        <xsl:apply-templates/>
                  </xsl:copy>
            </xsl:template>

            <xsl:template  match="*[name()='strongsref']">
                  <xsl:element  name="a" >
			<xsl:attribute name="href"><xsl:value-of  select="@language"/>:<xsl:value-of select="@strongs_number"/></xsl:attribute>
                        <xsl:apply-templates/>
                  </xsl:element>
            </xsl:template>

            <xsl:template  match="*[name()='see']">
		<br/>see
                  <xsl:element  name="a" >
			<xsl:attribute name="href"><xsl:value-of  select="@language"/>:<xsl:value-of select="@strongs_number"/></xsl:attribute>
                        <xsl:apply-templates/>
                  </xsl:element>
            </xsl:template>


            <xsl:template  match="*[name()='latin']">
                  <xsl:element  name="i" >
                        <xsl:apply-templates/>
                  </xsl:element>
            </xsl:template>

            <xsl:template  match="*[name()='strongs_definition']">
                  <xsl:element  name="b" >
                        <xsl:apply-templates/>
                  </xsl:element>
            </xsl:template>

            <xsl:template  match="*[name()='kjv_definition']">
		 <h3><u>KJV Definition</u></h3>
		 <br/>
                  <xsl:element  name="b" >
                        <xsl:apply-templates/>
                  </xsl:element>
		  <br/>
            </xsl:template>

      </xsl:stylesheet> 
