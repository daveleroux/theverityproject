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
		<span style="font-family: SBL Hebrew; font-size:x-large">
		<xsl:choose>
			<xsl:when test="@src">
				<xsl:element  name="a" >
					<xsl:attribute name="style">text-decoration:none
					</xsl:attribute>
					<xsl:attribute name="href">hebrew://<xsl:value-of select="@src"/></xsl:attribute>  
					<xsl:value-of select="@lemma"/>                     
   		               </xsl:element>
			</xsl:when>
			<xsl:otherwise>
				<xsl:value-of select="@lemma"/>
			</xsl:otherwise>
		</xsl:choose>
		</span>
		<xsl:if test="@morph">
			<br/>
			<xsl:variable name="morphs" select="tokenize(@morph,' ')"/>

			<xsl:for-each select="$morphs">
     			<xsl:variable name="mymorph" select="."/>
				<b>
				<xsl:if test="$mymorph = 'a'">Adjective</xsl:if>
				<xsl:if test="$mymorph = 'a-f'">Adjective Feminine</xsl:if>
				<xsl:if test="$mymorph = 'a-m'">Adjective Masculine</xsl:if>
				<xsl:if test="$mymorph = 'adv'">Adverb</xsl:if>
				<xsl:if test="$mymorph = 'conj'">Conjunction</xsl:if>
				<xsl:if test="$mymorph = 'dp'">Demonstrative Particle</xsl:if>
				<xsl:if test="$mymorph = 'd'">Demonstrative Pronoun</xsl:if>
				<xsl:if test="$mymorph = 'x'">Indefinite Pronoun</xsl:if>
				<xsl:if test="$mymorph = 'inj'">Interjection</xsl:if>
				<xsl:if test="$mymorph = 'i'">Interrogative Pronoun</xsl:if>
				<xsl:if test="$mymorph = 'np'">Negative Particle</xsl:if>
				<xsl:if test="$mymorph = 'n'">Noun</xsl:if>
				<xsl:if test="$mymorph = 'n-f'">Noun Feminine</xsl:if>
				<xsl:if test="$mymorph = 'n-m'">Noun Masculine</xsl:if>
				<xsl:if test="$mymorph = 'n-m-loc'">Noun Masculine Location</xsl:if>
				<xsl:if test="$mymorph = 'prt'">Particle</xsl:if>
				<xsl:if test="$mymorph = 'p'">Personal Pronoun</xsl:if>
				<xsl:if test="$mymorph = 'prep'">Preposition</xsl:if>
				<xsl:if test="$mymorph = 'pron'">Pronoun</xsl:if>
				<xsl:if test="$mymorph = 'n-pr'">Proper Name</xsl:if>
				<xsl:if test="$mymorph = 'n-pr-f'">Proper Name Feminine</xsl:if>
				<xsl:if test="$mymorph = 'n-pr-loc'">Proper Name Location</xsl:if>
				<xsl:if test="$mymorph = 'n-pr-m'">Proper Name Masculine</xsl:if>
				<xsl:if test="$mymorph = 'r'">Relative Pronoun</xsl:if>
				<xsl:if test="$mymorph = 'v'">Verb</xsl:if>	
				</b>		
				<br/>
			</xsl:for-each>
			<br/>
		</xsl:if>
	</xsl:template>
	
<!--	<xsl:template match="w[@ID]">
		<xsl:value-of select="@morph"/>		
	</xsl:template>
-->
	
	<xsl:template match="note[@type='exegesis']">
		<xsl:apply-templates/>
		<br/>
	</xsl:template>
		
	<xsl:template match="note[@type='explanation']">
		<h4><u>Explanation</u></h4> <br/>
		<xsl:apply-templates/>
		<br/>
	</xsl:template>
	
	<xsl:template match="note[@type='translation']">
		<h4><u>Translation</u></h4> <br/>
		<xsl:apply-templates/>	
		<br/>	
	</xsl:template>
		
	<xsl:template match="hi">
		<b>
		<xsl:apply-templates/>		
		</b>
	</xsl:template>

	<xsl:template match="foreign">
<!--		[Greek:<xsl:apply-templates/>]  -->
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
